pkgname=timez
pkgver=1.0
pkgrel=1
pkgdesc="A powerful tool for measuring command execution time and resource usage."
arch=('x86_64')
url="https://github.com/veloXm/Timez"
source=('timez' 'timez_s')
license=('GPL3')
depends=('gcc')
sha256sums=('34f33625b2af0601bf1a89a8adb98fdab45d2631986eab1a3882d2ff6526ce37'
    '06a62b6d7848044aa4ab554d97b7ff82148800aa4894dacd50391e3d73e291dd')

maintainer="Mitesh Soni <smiteshhc@gmail.com>"
author="Mitesh Soni"

package() {
    echo "Installing timez...\n"
    sudo install -Dm755 'timez' '/usr/bin/timez'
    sudo install -Dm755 'timez_s' '/usr/bin/timez_s'
}
