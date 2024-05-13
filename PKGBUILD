pkgname=timez
pkgver=1.0
pkgrel=1
pkgdesc="A powerful tool for measuring command execution time and resource usage."
arch=('x86_64')
url="https://github.com/veloXm/Timez"
source=('timez' 'timez_s')
license=('GPL3')
depends=('gcc')
sha256sums=('263d537953a41ff0a106ac046c1a1b641bcf8c7d654fb1f93a306f04f96c0883'
    'ebf88d8f0d8846a26ae351ac36def2f417a8c530427f9e3bf22ec9a625d2c098')

maintainer="Mitesh Soni <smiteshhc@gmail.com>"
author="Mitesh Soni"

package() {
    echo "Installing timez...\n"
    sudo install -Dm755 'timez' '/usr/bin/timez'
    sudo install -Dm755 'timez_s' '/usr/bin/timez_s'
}
