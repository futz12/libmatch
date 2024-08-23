from . import lib

print('卸载libmatch中...')

lib.unregister_vulkan.argtypes = []
lib.unregister_vulkan.restype = None

lib.unregister_vulkan()
