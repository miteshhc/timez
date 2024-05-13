pkgname=timez
pkgver=1.0
pkgrel=1
pkgdesc="A powerful tool for measuring command execution time and resource usage."
arch=('x86_64')
url="https://github.com/veloXm/Timez"
source=('timez' 'timez_s')
license=('GPL3')
depends=('gcc')
sha256sums=('b63265000f4f3c84aa8d3d754a663a2bcf11dc5aca5ff682a637b9ae23439aa7'
    '990332cb8f72add343d558b86f8e284d80355bde309e71bcc9fcdb5a1876311e')

maintainer="Mitesh Soni <smiteshhc@gmail.com>"
author="Mitesh Soni"

package() {
    echo "Installing timez...\n"
    sudo install -Dm755 'timez' '/usr/bin/timez'
    sudo install -Dm755 'timez_s' '/usr/bin/timez_s'
}
