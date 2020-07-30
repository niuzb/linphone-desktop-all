
I fetch all the repos Linphone Desktop used and commit it here. hope it helps.

# Linphone Desktop

Linphone is an open source softphone for voice and video over IP calling and instant messaging.

It is fully SIP-based, for all calling, presence and IM features.

General description is available from [Linphone web site](https://www.linphone.org/technical-corner/linphone)

## Compile on Mac

1. Install XCode from the Apple store. Run it at least once to allow it to install its tools.

2. Install Homebrew by following the instructions here https://brew.sh/

3. Install dependencies:

        brew install cmake qt git

4. First ensure you have installed python2.7 and pip. You can get it for python 2.7 [there](https://stackoverflow.com/questions/34886101/how-to-install-pip-to-python-2-7-10-on-mac#34886254).

5. install python package:

        pip install pystache
        pip install graphviz
        pip install doxygen
        pip install pystache --user
        pip install six --user

6. Build :

        cd linphone-desktop-all
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_PARALLEL_LEVEL=10 -DCMAKE_BUILD_TYPE=RelWithDebInfo
        cmake --build . --target install --parallel 10 --config RelWithDebInfo
       ./OUTPUT/Linphone.app/Contents/MacOS/linphone --verbose

7. The project folder will be in the build directory and binaries should be in the OUTPUT folder.

8. When updating the project, the next build steps are a bit different:
     
        git fetch
        git pull --rebase
        git submodule update --init --recursive
        cmake --build . --target all --parallel 10 --config RelWithDebInfo
        cmake --install .

    OR

         cmake --build . --target install --parallel 10 --config RelWithDebInfo


## Compile on the Windows platform

1. Ensure that you have downloaded the Qt msvc2015 version or Qt msvc2017 version (32-bit). (64-bit version is not supported at this moment by Linphone Desktop.)
  - MinGW : [download](https://sourceforge.net/projects/mingw/)
    - Select all installer options except Ada and Fortran
    - Install it in the default location (C:/Mingw), this is important as there are hard-links on it.
    - The gcc version should be 6.3.0. It wasn't tested for other versions. It seems that MinGW from osdn.net try to install gcc 9 that breaks the build.
  - Yasm
    - download yasm-1.3.0-win32.exe
    - copy it to a bin directory of your user directory
    - rename yasm-1.3.0-win32.exe as yasm.exe
  - nasm : [download](https://www.nasm.us/pub/nasm/releasebuilds/2.14.02/win64/)
  - git : [download](https://git-scm.com/download/win)

Visual Studio must also be properly configured with addons. Under "Tools"->"Obtain tools and features", make sure that the following components are installed:
  - Tasks: Select Windows Universal Platform development, Desktop C++ Development, .NET Development
  - Under "Installation details". Go to "Desktop C++ Development" and add "SDK Windows 8.1 and SDK UCRT"
  - Individual component: Windows 8.1 SDK

2. Or open a Command line with Visual Studio Developer Command Prompt for VS 2017 and call qtenv2.bat that is in your qt binaries eg: C:\Qt\<version>\msvc2017\bin\qtenv2.bat

3. Install msys-coreutils : mingw-get install msys-coreutils

4. First ensure you have installed python2.7 and pip. You can get it for python 2.7 [there](https://stackoverflow.com/questions/34886101/how-to-install-pip-to-python-2-7-10-on-mac#34886254).

5. install python package:

        pip install pystache
        pip install graphviz
        pip install doxygen
        pip install pystache --user
        pip install six --user

4. Build as usual with adding -A Win32 to cmake .. (General Steps) :
        
        cmake .. -DCMAKE_BUILD_PARALLEL_LEVEL=10 -DCMAKE_BUILD_TYPE=RelWithDebInfo -A Win32
        cmake --build . --target ALL_BUILD --parallel 10 --config RelWithDebInfo

5. The project folder will be in the build directory and binaries should be in the OUTPUT folder.

8. When updating the project, the next build steps are a bit different:
     
        git fetch
        git pull --rebase
        git submodule update --init --recursive
        cmake --build . --target all --parallel 10 --config RelWithDebInfo
        cmake --install .

    OR

         cmake --build . --target install --parallel 10 --config RelWithDebInfo

## Installing Linux dependencies


Dependencies from 4.1 version of Desktop (refer it only if you have issues):

apt-get install libqt53dcore5:amd64 libqt53dextras5:amd64 libqt53dinput5:amd64 libqt53dlogic5:amd64 libqt53dquick5:amd64 libqt53dquickextras5:amd64 libqt53dquickinput5:amd64 libqt53dquickrender5:amd64  libqt53drender5:amd64 libqt5concurrent5:amd64 libqt5core5a:amd64 libqt5dbus5:amd64 libqt5designer5:amd64 libqt5designercomponents5:amd64 libqt5gui5:amd64 libqt5help5:amd64 libqt5multimedia5:amd64 libqt5multimedia5-plugins:amd64 libqt5multimediawidgets5:amd64 libqt5network5:amd64 libqt5opengl5:amd64 libqt5opengl5-dev:amd64 libqt5positioning5:amd64 libqt5printsupport5:amd64 libqt5qml5:amd64 libqt5quick5:amd64 libqt5quickcontrols2-5:amd64 libqt5quickparticles5:amd64 libqt5quicktemplates2-5:amd64 libqt5quicktest5:amd64 libqt5quickwidgets5:amd64 libqt5script5:amd64 libqt5scripttools5:amd64 libqt5sensors5:amd64 libqt5serialport5:amd64 libqt5sql5:amd64 libqt5sql5-sqlite:amd64 libqt5svg5:amd64 libqt5svg5-dev:amd64 libqt5test5:amd64 libqt5webchannel5:amd64 libqt5webengine-data libqt5webenginecore5:amd64 libqt5webenginewidgets5:amd64 libqt5webkit5:amd64 libqt5widgets5:amd64 libqt5x11extras5:amd64  libqt5xml5:amd64 libqt5xmlpatterns5:amd64 qt5-default:amd64 qt5-doc qt5-gtk-platformtheme:amd64 qt5-qmake:amd64 qt5-qmltooling-plugins:amd64

### License

Copyright © Belledonne Communications

Linphone is dual licensed, and is available either :

 - under a [GNU/GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html), for free (open source). Please make sure that you understand and agree with the terms of this license before using it (see LICENSE file for details).

 - under a proprietary license, for a fee, to be used in closed source applications. Contact [Belledonne Communications](https://www.linphone.org/contact) for any question about costs and services.

### Documentation

- [Supported features and RFCs](https://www.linphone.org/technical-corner/linphone/features)

- [Linphone public wiki](https://wiki.linphone.org/xwiki/wiki/public/view/Linphone/)


### Set your environment

1. It's necessary to install the pip command and to execute:

        pip install pystache

2. You have to set the environment variable Qt5_DIR to point to the path containing the cmake folders of Qt5, and the PATH to the Qt5 bin. Example:

        Qt5_DIR="~/Qt/5.12.5/gcc_64/lib/cmake"
        PATH="~/Qt/5.12.5/gcc_64/bin/:$PATH"

Note: If you have qtchooser set in your PATH, the best use is :

        eval "$(qtchooser -print-env)"
        export Qt5_DIR=${QTLIBDIR}/cmake/Qt5
        export PATH=${QTTOOLDIR}:$PATH

	
### Notes:

The build is done by building the SDK and the application. Their targets are sdk and linphone-qt.		

1. Create your build folder at the root of the project : mkdir build
Go to this new folder and begin the build process : cd build

2. Prepare your options : cmake ... By default, it will try compile all needed dependencies. You can remove some by adding -DENABLE_<COMPONENT>=NO to the command. You can use cmake-gui .. if you want to have a better access to them. You can add -DCMAKE_BUILD_PARALLEL_LEVEL=<count> to do <count> parallel builds for speeding up the process.
Also, you can add -DENABLE_BUILD_VERBOSE=ON to get more feedback while generating the project.

Note : For Makefile or Ninja, you have to add -DCMAKE_BUILD_TYPE=<your_config> if you wish to build in a specific configuration (for example RelWithDebInfo).

3. Build and install the whole project : cmake --build . --target <target> --parallel <count> (replace <target> with the target name and <count> by the number of parallel builds).

Note : For XCode or Visual Studio, you have to add --config <your_config> if you wish to build in a specific configuration (for example RelWithDebInfo).

When all are over, the files will be in the OUTPUT folder in the build directory. When rebuilding, you have to use cmake --build . --target install (or cmake --install .) to put the application in the correct configuration.

4. When doing some modifications in the SDK, you can rebuild only the SDK with the target sdk and the same for the application with linphone-qt-only

5. In order to get packages, you can use cmake .. -DENABLE_APP_PACKAGING=YES. The files will be in OUTPUT/packages folder.



#### General Troubleshooting

* The latest version of Doxygen may not work with the SDK. If you some build issues and have a specific version of Doxygen that is not in your PATH, you can use -DLINPHONESDK_DOXYGEN_PROGRAM.

Eg on Mac : -DLINPHONESDK_DOXYGEN_PROGRAM=/Applications/Doxygen.app/Contents/Resources/doxygen

* If the build of the SDK crash with something like "cmd.exe failed" and no more info, it can be a dependency that is not available. You have to check if all are in your PATH.
Usually, if it is about VPX or Decaf, this could come from your Perl installation.

* If the application doesn't start and create an empty file with a random name, it could be come from a bad configuration between your application and others sub-modules. Check your configurations and force them with -DCMAKE_BUILD_TYPE=<your_config> or --config <your_config>.


