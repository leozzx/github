# -*- mode: python -*-
a = Analysis(['moonbbs.py'],
             pathex=['/Users/leo/Documents/python/moonbbs'],
             hiddenimports=[],
             hookspath=None,
             runtime_hooks=None)
a.datas += [('chromedriver','./chromedriver','DATA')]
pyz = PYZ(a.pure)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='moonbbs',
          debug=False,
          strip=None,
          upx=True,
          console=False )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=None,
               upx=True,
               name='moonbbs')
app = BUNDLE(coll,
             name='moonbbs.app',
             icon=None)
