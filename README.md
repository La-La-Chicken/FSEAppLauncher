# FSE App Launcher for Windows

---

FSE App Launcher is a lightweight app launcher compatible with full screen experience (FSE)[^1] on Windows 11.

## Highlights

- Provides the native list view and right-click menu based on Windows Shell, with an experience similar to the "All apps" list in the Start menu.

- Supports setting it as a home app on devices that support the full screen experience feature (see "System Requirements").

## System Requirements

##### Minimum requirements

- Windows 11, version 24H2+

##### Other requirements (if you intend to set the application as home app of full screen experience)

- Windows 11, version 25H2+
- The device is a gaming handheld

## Installation and Configuration

##### 1. Download package from Github

- Go to the Releases page, click Assets to reveal the downloads, and download the archive.
- Extract the archive to any directory of your choice.

##### 2. Install the developer certificate

- Right-click (or press and hold on) the security certificate file (usually "FSEAppLauncher.WAP...x64.cer"), select "Show more options", then select "<u>I</u>nstall Certificate".

![image-20260220000902249](C:/Users/YXY/AppData/Roaming/Typora/typora-user-images/image-20260220000902249.png)

- Select "<u>L</u>ocal Machine", then click "<u>N</u>ext".

![uwp-cert-2](C:/Users/YXY/AppData/Local/Temp/uwp-cert-2.png)

- If the User Account Control dialog appears, click "Yes" to approve the request.

- Select "<u>P</u>lace all certificates in the following store", then click "B<u>r</u>owse...".

![uwp-cert-3](C:/Users/YXY/AppData/Local/Temp/uwp-cert-3.png)

- Select "Trusted Root Certification Authorities", then click "OK".

![uwp-cert-4](C:/Users/YXY/AppData/Local/Temp/uwp-cert-4.png)

- Click "<u>N</u>ext", then click "<u>F</u>inish".

##### 3. Install the dependency

Double click the dependency "Microsoft.VCLibs.x64.14.00.Desktop.appx" and then click "Install".

##### 4. Install the application

After installing the certificate and dependency, you can install the application by double clicking "FSEAppLauncher.WAP...x64.msix" and then click "Install".

##### 5. Configure the application as the home app

Open System Settings, click "Gaming", then click "Full screen experience"[^2].

Beside "Choose home app", select "App Launcher for Windows", and enable "Enter full screen experience on startup"[^2].

Optionally enable "Show accessibility control hints in Task View" based on your needs.

## What's new

##### Version 26.23.27360.0 (Feb 2026)

Initial release.

## Developing and debugging

Use the toolchain to develop:

- Windows 10, version 1607+

- Microsoft Visual Studio 2017+
	- C++ desktop development
		- MSVC v141
		- Windows 10 SDK (10.0.19041)
		- C++ MFC development tools

**Windows 11, version 24H2+ is required to run or debug the packaged application.**

## Contributing

This project welcomes contributions of all types, including new features, bug fixes, UI / UX redesign, documentation, etc.

## Privacy

The application needs to access your app list to display the "Apps" page.

Your user information will not be sent to any organization or individual.

## License

This project is licensed under the Unlicense.

This means unlicensed works, modifications, and larger works may be distributed under different terms and without source code.



[^1]: Also known as "Xbox full screen experience" and "Xbox FSE". In fact, "Xbox" is not part of the official name of this feature. This prefix depends on the home app chosen.
[^2]: If your operating system version is lower than Windows 11, version 25H2, or your device is not a gaming handheld, you may not see "Full screen experience", and some options may not appear.