# 
# Do not Edit! Generated by:
# spectacle version 0.18
# 
# >> macros
%define _unpackaged_files_terminate_build 0
# << macros

Name:       meegotouch-controlpanelapplets
Summary:    Various MeeGo Touch Controlpanel Applets
Version:    0.15.4
Release:    1
Group:      System/GUI/Other
License:    LGPL v2.1
URL:        http://meego.gitorious.org/meegotouch/meegotouch-controlpanelapplets
Source0:    %{name}-%{version}.tar.bz2
Source100:  meegotouch-controlpanelapplets.yaml
BuildRequires:  pkgconfig(meegotouch)
BuildRequires:  pkgconfig(QtCore) >= 4.7.0
BuildRequires:  pkgconfig(QtGui) >= 4.7.0
BuildRequires:  meegotouch-controlpanel-devel
BuildRequires:  libqttracker-devel
BuildRequires:  meegotouch-systemui-devel
BuildRequires:  libthumbnailer-devel


%description
Various Controlpanel applets for MeeGo Touch environment:
    - Offline switcher
    - USB settings
    - Battery info/settings
    - Display settings
    - Profiles settings
    - Theme-switcher
    - Wallpaper editor/setter
    - Clear-Device & Restore-Factory-Settings
    - About product
    - Warranty applet
    - Profiles switcher



%package -n meegotouchcp-applets-l10n-eng-en
Summary:    MeeGo Touch Controlpanel applets engineering English messages
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}
Provides:   meegotouchcp-applets-l10n

%description -n meegotouchcp-applets-l10n-eng-en
MeeGo Touch Controlpanel applets engineering English messages for various applets:
    - Offline switcher
    - USB settings
    - Battery info/settings
    - Display settings
    - Profiles settings
    - Theme-switcher
    - Wallpaper editor/setter
    - Clear-Device & Restore-Factory-Settings
    - About product
    - Warranty applet
    - Profiles switcher


%package -n meegotouchcp-profiles
Summary:    MeegoTouch Controlpanel profiles settings
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-profiles
MeegoTouch Controlpanel profiles settings

%package -n meegotouchcp-wallpaper
Summary:    MeegoTouch Controlpanel wallpaper editor & setter
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}
Requires:   tumbler

%description -n meegotouchcp-wallpaper
MeegoTouch Controlpanel wallpaper editor & setter

%package -n meegotouchcp-about
Summary:    MeegoTouch Controlpanel about product applet
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-about
MeegoTouch Controlpanel about product applet

%package -n meegotouchcp-usb
Summary:    MeegoTouch Controlpanel USB settings
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-usb
MeegoTouch Controlpanel USB settings

%package -n meegotouchcp-reset
Summary:    MeegoTouch Controlpanel reset-settings & clear-user data
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-reset
MeegoTouch Controlpanel reset-settings & clear-user data

%package -n meegotouchcp-battery
Summary:    MeegoTouch Controlpanel battery info/settings
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-battery
MeegoTouch Controlpanel battery info/settings

%package -n meego-handset-statusindicators-profiles
Summary:    MeegoTouch profile-switcher statusindicatormenu plugin
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}
Requires:   meegotouch-systemui

%description -n meego-handset-statusindicators-profiles
MeegoTouch profile-switcher statusindicatormenu plugin

%package -n meegotouchcp-warranty
Summary:    MeegoTouch Controlpanel warranty applet
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-warranty
MeegoTouch Controlpanel warranty applet

%package -n meegotouchcp-theme
Summary:    MeegoTouch Controlpanel theme switcher
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-theme
MeegoTouch Controlpanel theme switcher

%package -n meegotouchcp-offline
Summary:    MeegoTouch Controlpanel Offline switcher
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-offline
MeegoTouch Controlpanel Offline switcher

%package -n meegotouchcp-display
Summary:    MeegoTouch Controlpanel display settings
Group:      System/GUI/Other
Requires:   %{name} = %{version}-%{release}

%description -n meegotouchcp-display
MeegoTouch Controlpanel display settings


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
qmake
# << build pre


make %{?jobs:-j%jobs}

# >> build post
# << build post
%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%make_install 

# >> install post
rm -fR %{buildroot}
make INSTALL_ROOT=%{buildroot} install
install -m 644 -D -p LICENSE.LGPL %{buildroot}/usr/share/doc/packages/%{name}/LICENSE.LGPL

# << install post










































%files
%defattr(-,root,root,-)
%dir /usr/share/doc/packages/%{name}
%doc /usr/share/doc/packages/%{name}/*
# >> files
# << files


%files -n meegotouchcp-applets-l10n-eng-en
%defattr(-,root,root,-)
%dir /usr/share/l10n/meegotouch
/usr/share/l10n/meegotouch/systemui-applets.qm
# >> files meegotouchcp-applets-l10n-eng-en
# << files meegotouchcp-applets-l10n-eng-en

%files -n meegotouchcp-profiles
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
%dir /usr/share/themes/base/meegotouch/duicontrolpanel/style
/usr/lib/duicontrolpanel/profile.desktop
/usr/lib/duicontrolpanel/applets/libprofileapplet.so
/usr/share/themes/base/meegotouch/duicontrolpanel/style/profileapplet.css
# >> files meegotouchcp-profiles
# << files meegotouchcp-profiles

%files -n meegotouchcp-wallpaper
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
%dir /usr/share/themes/base/meegotouch/duicontrolpanel/style
/usr/lib/duicontrolpanel/wallpaper.desktop
/usr/lib/duicontrolpanel/applets/libwallpaperapplet.so
/usr/share/themes/base/meegotouch/duicontrolpanel/style/wallpaper.css
# >> files meegotouchcp-wallpaper
# << files meegotouchcp-wallpaper

%files -n meegotouchcp-about
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
/usr/lib/duicontrolpanel/about.desktop
/usr/lib/duicontrolpanel/applets/libaboutapplet.so
# >> files meegotouchcp-about
# << files meegotouchcp-about

%files -n meegotouchcp-usb
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
/usr/lib/duicontrolpanel/usbapplet.desktop
/usr/lib/duicontrolpanel/applets/libusbapplet.so
# >> files meegotouchcp-usb
# << files meegotouchcp-usb

%files -n meegotouchcp-reset
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
/usr/lib/duicontrolpanel/reset.desktop
/usr/lib/duicontrolpanel/applets/libresetapplet.so
# >> files meegotouchcp-reset
# << files meegotouchcp-reset

%files -n meegotouchcp-battery
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
%dir /usr/share/themes/base/meegotouch/duicontrolpanel/style
/usr/lib/duicontrolpanel/battery.desktop
/usr/lib/duicontrolpanel/applets/libbatteryapplet.so
/usr/share/themes/base/meegotouch/duicontrolpanel/style/batteryapplet.css
# >> files meegotouchcp-battery
# << files meegotouchcp-battery

%files -n meego-handset-statusindicators-profiles
%defattr(-,root,root,-)
%dir /usr/lib/meegotouch/applicationextensions
/usr/lib/meegotouch/applicationextensions/libstatusindicatormenu-profile.so
/usr/share/meegotouch/applicationextensions/statusindicatormenu-profile.desktop
# >> files meego-handset-statusindicators-profiles
# << files meego-handset-statusindicators-profiles

%files -n meegotouchcp-warranty
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
/usr/lib/duicontrolpanel/warranty.desktop
/usr/lib/duicontrolpanel/applets/libwarrantyapplet.so
# >> files meegotouchcp-warranty
# << files meegotouchcp-warranty

%files -n meegotouchcp-theme
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
%dir /usr/share/themes/base/meegotouch/duicontrolpanel/style
/usr/lib/duicontrolpanel/theme.desktop
/usr/lib/duicontrolpanel/applets/libthemeapplet.so
/usr/share/themes/base/meegotouch/duicontrolpanel/style/themeapplet.css
# >> files meegotouchcp-theme
# << files meegotouchcp-theme

%files -n meegotouchcp-offline
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
/usr/lib/duicontrolpanel/offline.desktop
/usr/lib/duicontrolpanel/applets/libofflineapplet.so
# >> files meegotouchcp-offline
# << files meegotouchcp-offline

%files -n meegotouchcp-display
%defattr(-,root,root,-)
%dir /usr/lib/duicontrolpanel
%dir /usr/lib/duicontrolpanel/applets
%dir /usr/share/themes/base/meegotouch/duicontrolpanel/style
/usr/lib/duicontrolpanel/display.desktop
/usr/lib/duicontrolpanel/applets/libdisplayapplet.so
/usr/share/themes/base/meegotouch/duicontrolpanel/style/display.css
# >> files meegotouchcp-display
# << files meegotouchcp-display

