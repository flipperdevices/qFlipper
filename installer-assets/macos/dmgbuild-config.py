import os.path

application = defines.get('app', False)
appname = os.path.basename(application)
format = 'UDZO'
files = [application]
symlinks = {'Applications': '/Applications'}
hide_extension = [application]
icon = '../installer-assets/macos/VolumeIcon.icns'
background = '../installer-assets/macos/background/qFlipper_disk_background.png'
icon_locations = {
    appname:        (125, 150),
    'Applications': (485, 150)
}
window_rect = ((200, 120), (600, 400))
