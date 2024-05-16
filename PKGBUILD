# Maintainer: Mitesh <smiteshhc@gmail.com>

pkgname=timez
pkgver=1.2
pkgrel=1
pkgdesc="A powerful tool for measuring command execution time and resource usage."
arch=('x86_64')
url="https://github.com/veloXm/Timez"
license=('GPL3')
depends=('gcc')
source=('timez' 'timez_s')
sha256sums=('bda028adc12928b87cda67095eb2e4d13646fa83a3f25883cf7593b8b841415f'
    'c50a2d1d5b39c6d9be64a947d02c5f4f14a829dc487c26a5b7ab9d595c90e82e')

package() {
    echo "Installing timez...\n"
    sudo install -Dm755 'timez' '/usr/bin/timez'
    sudo install -Dm755 'timez_s' '/usr/bin/timez_s'
}
