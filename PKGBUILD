pkgname=timez
pkgver=1.0
pkgrel=1
pkgdesc="A powerful tool for measuring command execution time and resource usage."
arch=('x86_64')
url="https://github.com/veloXm/Timez"
source=('timez' 'timez_s')
license=('GPL3')
depends=('gcc')
sha256sums=('ba8e670be224a75513e562b5fd9d4bbcccd2b2fcf8f9b1d76109329f57f7ac1b'
    'ceb3eb0d8f2aa3834ebcb9e57f7074aea76a9e2ebdbc8a7a64747eb6e073b2d5')

maintainer="Mitesh Soni <smiteshhc@gmail.com>"
author="Mitesh Soni"

package() {
    echo "Installing timez...\n"
    sudo install -Dm755 'timez' '/usr/bin/timez'
    sudo install -Dm755 'timez_s' '/usr/bin/timez_s'
}
