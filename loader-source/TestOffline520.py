"""Headless loopback tests of the extension. No loader/AMI/driver execution."""
import hashlib
import http.server
import io
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import threading
import zipfile

build = Path(sys.argv[1]).resolve()
evidence = []
local_hash = '24DD330F14BE62CDED3D1837124014FBFF2ABDF6D209BD4893C931064B50604D'
online = io.BytesIO()
with zipfile.ZipFile(online, 'w', zipfile.ZIP_DEFLATED) as archive:
    archive.writestr('fixture.txt', 'online fixture, never executed')
online = online.getvalue()
mode = 'online'
counts = {}

class Handler(http.server.BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def do_GET(self):
        counts[self.path] = counts.get(self.path, 0) + 1
        if self.path == '/other':
            status, body = 404, b'unrelated-request-unchanged'
        elif mode == 'online':
            status, body = 200, online
        elif mode == 'notzip':
            status, body = 200, b'not a zip'
        elif mode == 'empty':
            status, body = 200, b''
        elif mode == 'partial':
            self.send_response(200)
            self.send_header('Content-Length', str(len(online) + 100))
            self.end_headers()
            self.wfile.write(online[:40])
            self.close_connection = True
            return
        else:
            status, body = int(mode), b'HTTP failure'
        self.send_response(status)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
base = f'http://127.0.0.1:{server.server_port}'
with socket.socket() as closed:
    closed.bind(('127.0.0.1', 0))
    closed_port = closed.getsockname()[1]
offline = f'http://127.0.0.1:{closed_port}/package'
canonical = 'https://pub-fdaae9442b754a829739a8850fcc81a5.r2.dev/ks_driver'

def check(name, dll, url, expected_hash=None, api='A', target=None, proxy=None):
    env = os.environ.copy()
    env.pop('LOADER_FIXTURE_PROXY', None)
    env['LOADER_TEST_PACKAGE_URL'] = target or url
    if proxy:
        env['LOADER_FIXTURE_PROXY'] = proxy
    run = subprocess.run([str(build / 'fixture' / 'TestDriverDownload.exe'), str(dll), url, api],
                         capture_output=True, text=True, timeout=45, env=env)
    result = run.stdout.strip()
    passed = run.returncode == 0 and f'sha256={expected_hash}' in result if expected_hash else run.returncode == 10
    record = dict(name=name, passed=passed, exit=run.returncode, result=result, error=run.stderr.strip())
    evidence.append(record)
    print(json.dumps(record), flush=True)
    if not passed:
        raise AssertionError(name)

try:
    fixture = build / 'fixture' / 'wininet.dll'
    production = build / 'package' / 'wininet.dll'
    check('online-A', fixture, base + '/package', hashlib.sha256(online).hexdigest().upper())
    check('online-W', fixture, base + '/package', hashlib.sha256(online).hexdigest().upper(), api='W')
    check('online-without-local-package', build / 'missing-local' / 'wininet.dll', base + '/package', hashlib.sha256(online).hexdigest().upper())
    check('online-with-corrupt-local-package', build / 'corrupt-local' / 'wininet.dll', base + '/package', hashlib.sha256(online).hexdigest().upper())
    for mode in ['404', '500', 'notzip', 'empty', 'partial']:
        check(f'fallback-{mode}', fixture, base + '/package', local_hash)
    check('fallback-connection-failed-A', fixture, offline, local_hash)
    check('fallback-connection-failed-W', fixture, offline, local_hash, api='W')
    check('unrelated-url-pass-through', fixture, base + '/other', hashlib.sha256(b'unrelated-request-unchanged').hexdigest().upper(), target=base + '/package')
    check('missing-local-package-fails', build / 'missing-local' / 'wininet.dll', offline)
    check('corrupt-local-package-fails', build / 'corrupt-local' / 'wininet.dll', offline)
    check('production-ignores-test-url-override', production, base + '/other', hashlib.sha256(b'unrelated-request-unchanged').hexdigest().upper())
    check('production-canonical-offline-A', production, canonical, local_hash, proxy=f'127.0.0.1:{closed_port}')
    check('production-canonical-offline-W', production, canonical, local_hash, api='W', proxy=f'127.0.0.1:{closed_port}')
    print(json.dumps(dict(passed=len(evidence), loopbackRequests=counts, hardwareExecuted=False)), flush=True)
finally:
    server.shutdown()
