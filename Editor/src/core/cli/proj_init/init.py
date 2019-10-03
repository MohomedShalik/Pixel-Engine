import os,shutil

import proj_updater
import assets_updater
import register_updater

## src
pe_sln_path     = 'C:/dev/Pixel-Engine'
pe_api_path     = 'C:/dev/Pixel-Engine/Pixel-Engine/src/api/'
sfml_path       = 'C:/dev/Pixel-Engine/vender/SFML-2.5.1/include/'
sfml_lib_path   = 'C:/dev/Pixel-Engine/vender/SFML-2.5.1/lib/'
sfml_dll_path   = 'C:/dev/Pixel-Engine/vender/SFML-2.5.1/bin/'
py_dll_path     = 'C:/dev/Pixel-Engine/vender/pybind/dll'
pe_debug_out    = 'C:/dev/Pixel-Engine/bin/Debug-x64/Pixel-Engine/'
pe_release_out  = 'C:/dev/Pixel-Engine/bin/Release-x64/Pixel-Engine/'
#res_path        = "C:/dev/Pixel-Engine/Editor/res/"

init_file_name = "conf.init"
init_format = '''\
conf="%s"
cwd="%s"
log="%s"
kill_switch="F9"
'''


## /MDd : defines _DEBUG, _MT, _DLL
## /Fe output File exe name
## cl /EHsc /DEBUG /MDd /Febin/x64-debug/SandBox /Iinclude link.cpp
## cl /EHsc /Febin/x64-release/SandBox /Iinclude link.cpp

link_file_name = 'link.cpp'
link_cpp = '''\
#define PE_PROJECT %s
#include "register.h"

#ifdef _DEBUG
	#pragma comment(lib, "lib/x64-debug/Pixel-Engine.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-audio-d.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-graphics-d.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-main-d.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-network-d.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-system-d.lib")
	#pragma comment(lib, "lib/x64-debug/sfml-window-d.lib")
#else
	#pragma comment(lib, "lib/x64-release/Pixel-Engine.lib")
	#pragma comment(lib, "lib/x64-release/sfml-audio.lib")
	#pragma comment(lib, "lib/x64-release/sfml-graphics.lib")
	#pragma comment(lib, "lib/x64-release/sfml-main.lib")
	#pragma comment(lib, "lib/x64-release/sfml-network.lib")
	#pragma comment(lib, "lib/x64-release/sfml-system.lib")
	#pragma comment(lib, "lib/x64-release/sfml-window.lib")
#endif
'''

def makeInit(dst, conf, cwd, log):
    file = open(os.path.join(dst, init_file_name), 'w')
    file.write( init_format%(conf, cwd, log) )
    file.close()
    

def makeDirs(proj_dir):
    os.makedirs(proj_dir)
    os.makedirs(os.path.join(proj_dir, 'include'))
    os.makedirs(os.path.join(proj_dir, 'lib/x64-debug'))
    os.makedirs(os.path.join(proj_dir, 'lib/x64-release'))
    os.makedirs(os.path.join(proj_dir, 'bin/x64-debug'))
    os.makedirs(os.path.join(proj_dir, 'bin/x64-release'))
    os.makedirs(os.path.join(proj_dir, 'src'))
    os.makedirs(os.path.join(proj_dir, 'res'))
    print('project dirs created')

def copyInclude(src, dst):
    copyTree(src, dst, dir_override=True)
    for path, dirs, files in os.walk(dst):
        for file in files:
            if file.endswith('.cpp'):
                os.remove( os.path.join(path, file) )
                print("removed file: %s"%file)

def clearLib(dst):
    for path, dirs, files in os.walk(dst):
        for file in files:
            if file.endswith('.pdb'):
                os.remove( os.path.join(path, file) )
                print("removed file: %s"%file)

def copyTree(src, dst, symlinks=False, ignore=None, dir_override=False):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            try:
                shutil.copytree(s, d, symlinks, ignore)
                print("copied dir: %s"%d)
            except:
                if dir_override:
                    shutil.rmtree(d)
                    shutil.copytree(s, d, symlinks, ignore)
                    print("copied dir: %s"%d)
                else:
                    print("Error : directory already exists!")
        else:
            shutil.copy2(s, d)
            print("copied file: %s"%d)
        

def copySfmlLibs(sfml_lib_path, proj_dir):
    for lib in os.listdir(sfml_lib_path):
        if lib.split('.')[0][-1] == 'd':
            shutil.copy(os.path.join(sfml_lib_path, lib),
                        os.path.join(proj_dir, 'lib/x64-debug'))
            print("copied file:",os.path.join(proj_dir, 'lib/x64-debug/%s'%lib))
        else:
            shutil.copy(os.path.join(sfml_lib_path, lib),
                        os.path.join(proj_dir, 'lib/x64-release'))
            print("copied file:",os.path.join(proj_dir, 'lib/x64-release/%s'%lib))
    clearLib(os.path.join(proj_dir, 'lib'))

def copySfmlDlls(sfml_dll_path, proj_dir):
    for dll in os.listdir(sfml_dll_path):
        if dll == 'openal32.dll':
            shutil.copy(os.path.join(sfml_dll_path, dll),
                        os.path.join(proj_dir, 'bin/x64-debug'))
            shutil.copy(os.path.join(sfml_dll_path, dll),
                        os.path.join(proj_dir, 'bin/x64-release'))
            print("copied file:",os.path.join(sfml_dll_path, dll))
            continue
        if dll.endswith('-d-2.dll'):
            shutil.copy(os.path.join(sfml_dll_path, dll),
                        os.path.join(proj_dir, 'bin/x64-debug'))
            print("copied file:",os.path.join(proj_dir, 'bin/x64-debug%s'%dll))
        else:
            shutil.copy(os.path.join(sfml_dll_path, dll),
                        os.path.join(proj_dir, 'bin/x64-release'))
            print("copied file:",os.path.join(proj_dir, 'bin/x64-release%s'%dll))

def copyPythonDll(py_dll_path, proj_dir, spec='debug'):
    assert( spec in ['debug', 'release'] )
    dll_name = 'python3.dll' #if spec == 'release' else 'python3_d.dll'
    ## todo: for debug copy python3_d.dll (not support in my pc)
    shutil.copy(os.path.join(py_dll_path, dll_name),
                        os.path.join(proj_dir, 'bin/x64-%s'%spec))
    print("copied file:",os.path.join(proj_dir, 'bin/x64-%s'%spec))

def copyPEOut(pe_debug_out, proj_dir, spec='debug'):
    assert( spec in ['debug', 'release'] )
    shutil.copy(os.path.join(pe_debug_out, 'Pixel-Engine.dll'),
                        os.path.join(proj_dir, 'bin/x64-%s'%spec))
    print("copied file:",os.path.join(proj_dir, 'bin/x64-%s/Pixel-Engine.dll'%spec))
    shutil.copy(os.path.join(pe_debug_out, 'Pixel-Engine.lib'),
                        os.path.join(proj_dir, 'lib/x64-%s'%spec))
    print("copied file:",os.path.join(proj_dir, 'lib/x64-%s/Pixel-Engine.lib'%spec))
    

def copyLicense(pe_sln_path, proj_dir):
    ##shutil.copy(os.path.join(pe_sln_path, 'SConstruct'), proj_dir)
    shutil.copy(os.path.join(pe_sln_path, 'LICENSE'),
                os.path.join(proj_dir, 'include'))
    print("copied file:",os.path.join(os.path.join(proj_dir, 'include/LICENSE')))

def copyResDir(res_path, dst):
    copyTree(res_path, dst)
    
#####################################################

def updateProj(proj_name="", proj_dir='.', include_pe=True):
    if include_pe:
        copyInclude(pe_api_path, os.path.join(proj_dir, 'include'))
        copyPEOut(pe_debug_out, proj_dir, 'debug')
        copyPEOut(pe_release_out, proj_dir, 'release')
    assets_updater.updateAssets(proj_dir)
    proj_updater.updateProj(proj_name, proj_dir)
    register_updater.updateRegister( proj_dir)

## if init success return 0
def init(__proj_name, __dst_path=None):
    if __proj_name[0].isnumeric():
        return 1
    for illegal in [' ','\t','\n']:
        if illegal in __proj_name:
            return 1
        
    proj_name = __proj_name
    dst_path = __dst_path if __dst_path else '.'
    proj_dir = os.path.join(dst_path, proj_name)
    try:
        makeDirs(proj_dir)
    except FileExistsError:
        print('ERROR: file already exists, initialization terminate!')
        return
    #copyResDir(res_path, os.path.join(proj_dir, 'res'))
    copyInclude(sfml_path, os.path.join(proj_dir, 'include'))
    copySfmlLibs(sfml_lib_path, proj_dir)
    copySfmlDlls(sfml_dll_path, proj_dir)
    copyPythonDll(py_dll_path, proj_dir, 'debug')
    copyPythonDll(py_dll_path, proj_dir, 'release')
    
    updateProj(proj_name, proj_dir)
    copyLicense(pe_sln_path, proj_dir)

    link_cpp_file = open( os.path.join( proj_dir,link_file_name), 'w')
    link_cpp_file.write(link_cpp%proj_name)
    link_cpp_file.close()
    
    makeInit( os.path.join( proj_dir, 'bin/x64-debug'), "debug", '../../', 'bin/x64-debug/log.txt' )
    makeInit( os.path.join( proj_dir, 'bin/x64-release'), "release", '../../', 'bin/x64-release/log.txt' )

    return 0


if __name__ == "__main__" and 0:
    if 1:
        init("testproj1","E:/__test/test")
    else:
        updateProj("SlrcPlot","E:/__test/test/SlrcPlot")
    
    
    
    










