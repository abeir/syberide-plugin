#!/bin/sh

DR=0
PROJECTNAME=""
DSC="Missing description"
EXT_TYPE=3rd
OBJECTNAME=""
URI=""
DPREFIX=/data

# awk loops concatenate strings that contained = back to original.
for A in $*
do
    case $A in
    --project-name=*)
        PROJECTNAME=$(echo $A | awk -F= '{ printf("%s", $2); for (k = 3; k <= NF; k++) printf("=%s", $k); printf("\n") }')
        ;;
    --description=*)
        DSC=$(cat $(echo $A | awk -F= '{ printf("%s", $2); for (k = 3; k <= NF; k++) printf("=%s", $k); printf("\n") }'))
        ;;
    --object-name=*)
        OBJECTNAME=$(echo $A | awk -F= '{ printf("%s", $2); for (k = 3; k <= NF; k++) printf("=%s", $k); printf("\n") }')
        ;;
    --exttype=*)
        EXT_TYPE=$(echo $A | awk -F= '{ printf("%s", $2); for (k = 3; k <= NF; k++) printf("=%s", $k); printf("\n") }')
        ;;
    --uri*)
        URI=$(echo $A | awk -F= '{ printf("%s", $2); for (k = 3; k <= NF; k++) printf("=%s", $k); printf("\n") }')
        ;;
    --dry-run)
        DR=1
        ;;
    esac
done

if [ $DR -eq 1 ]; then
    echo syberos.pri
#    echo rpm/$PROJECTNAME.spec
    exit 0
fi

# Change URI into relative path.
URIPATH=$(echo $URI | sed 's/\./\//g')

# The first word ends up as the value of the variable now. Pay attention to
# what reads in the wizard.xml. If the combo box gets localized then this
# may become a tad bit too unwieldy.
#PREFIX_PRE="#"
#PREFIX_3RD="#"
case $EXT_TYPE in
Pre-installed)
    EXT_TYPE="preinstalled"
    EXT_ROOT=/usr/lib/qt5/qml/$URIPATH
#    PREFIX_PRE="%"
    ;;
3rd)
    EXT_TYPE="thirdparty"
    EXT_ROOT=$DPREFIX/apps/not/in/$URIPATH
#    PREFIX_3RD="%"
    ;;
esac



cat > syberos.pri <<EOF
OBJECTNAME = $OBJECTNAME
EXTNAME = $PROJECTNAME
DPREFIX = ${DPREFIX}
URI = $URI
URIPATH = $URIPATH
EXT_ROOT = $DPREFIX/path/not/in/\$\$URIPATH/

EOF


# Comment out the following codes because they are no use for now.
:<<BLOCK

mkdir rpm
cat > rpm/$PROJECTNAME.spec <<EOF

%define _dprefix $DPREFIX
# Location for preinstalled system QtQuick plugins.
${PREFIX_PRE}define _extroot /usr/lib/qt5/qml
# Location for system QtQuick plugin updates.
#define _extroot %{_dprefix}/path/not/in/spec
# Location for third party QtQuick plugins.
${PREFIX_3RD}define _extroot %{_dprefix}/path/not/in/spec


Name:       $PROJECTNAME
Summary:    Short description
Version:    0.1.0
Release:    1
Group:      Libraries
License:    License information
URL:        http://ext.url/
# This must remain as is for OBS. Also build script expects this.
Source0:    %{name}-%{version}.tar.gz
# Add required packages, if any.
# Add requirements for building. OBS may need packages that are on SDK VM by
# default.
BuildRequires:  gcc-c++


%description
$DSC


%prep
%setup -q -n %{name}-%{version}


%build
# Macro has -qt option and qmake does not recognize it.
#qmake5 IDE_LIBRARY_BASENAME=%{_lib}
export QT_SELECT=5
qtchooser -run-tool=qmake -qt=5 -makefile "QMAKE_CFLAGS_RELEASE=\${CFLAGS:-%optflags}" "QMAKE_CFLAGS_DEBUG=\${CFLAGS:-%optflags}" "QMAKE_CXXFLAGS_RELEASE=\${CXXFLAGS:-%optflags}" "QMAKE_CXXFLAGS_DEBUG=\${CXXFLAGS:-%optflags}" QMAKE_STRIP=: IDE_LIBRARY_BASENAME=%{_lib}
make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_extroot}
%qmake5_install INSTALL_ROOT=%{buildroot}


%post


%postun


%files
%{_extroot}


%changelog


EOF

BLOCK

