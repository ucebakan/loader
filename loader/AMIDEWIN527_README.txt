AMIDEWIN 5.27 COMPATIBILITY BUILD
=================================

Run:
  4unityoffline-login-bypass-AMIDEWIN527.exe

Do not launch the compatibility proxy directly. The original files remain
unchanged and beside the compatibility launcher:
  4unityoffline-login-bypass.exe
  4unityofflinetestsunucusu.exe

At runtime, after the existing ks_driver package has been extracted, the
compatibility launcher changes only the AMIDEWIN file layer:

  C:\ks1\AMITools520\
      AMIDEWINx64.EXE
      amifldrv64.sys

  C:\ks1\AMITools527\
      AMIDEWINx64.EXE
      amifldrv64.sys
      amigendrv64.sys

The existing C:\ks1\AMITools\sdeven.sys file and the loader's driver/service
workflow are not changed. C:\ks1\AMITools\AMIDEWINx64.EXE becomes the small
compatibility proxy because the production loader source is not present in the
supplied package and its hardcoded call site cannot be rebuilt directly.

The proxy performs one centralized selection:
  1. Probe AMIDEWIN 5.27.00.0003 with /SM.
  2. Require a valid "(/SM)System manufacture ... R ... Done" result.
  3. Reject known SMBIOS initialization errors, start failures, crashes, and
     30-second timeouts.
  4. Fall back once to AMIDEWIN 5.20.0042 and probe it the same way.
  5. Cache the selected executable path for the extracted package.
  6. Forward every existing AMIDEWIN command to that one selected path.

No authentication, GUI, registry, disk, kernel driver/service, WMI, or download
implementation was changed.
