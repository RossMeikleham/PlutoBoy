#Release environment
env = Environment(CC = 'clang', 
                  CCFLAGS = ' -O3 -fomit-frame-pointer -W -Wall -Wextra')

env.Append(LIBS = ['SDL'])
env.Append(LINKFLAGS = '')

sourceObjs = env.Object( Glob('src/core/*.c'))\
           + env.Object( Glob('src/core/mmu/*.c'))\
           + env.Object( Glob('src/shared_libs/SDL/*.c'))\
           + env.Object( Glob('src/platforms/standard/*.c'))\
           + env.Object( Glob('src/shared_libs/SDL/*.cpp'))\
           + env.Object( Glob('src/shared_libs/SDL/audio/*.cpp'))


env.Program('gb_emu',sourceObjs)
