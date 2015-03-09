from distutils.core import setup

packages    = [ 'arc',
                'arc.util',
              ]

setup(name        = 'arc',
      version     = '2.0',
      description = 'Basic arc functions for easy test',
      author      = 'Meng Fanke',
      packages    = packages,
      requires    = [ 'sleekxmpp', 'pycrypto' ],
)
