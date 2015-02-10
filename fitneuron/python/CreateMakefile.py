#!/usr/bin/python
usageStr = \
"""CreateMakeFile.py targetBinary1 [target2 ...] [-shared sharedDir1 dir2 ...]
  Create a c++ Makefile in the current directory, given a list of target
  executable files.
   -To trigger the use of a compiler flag, add a commented line to the c++ file
    that requires it.  e.g. to turn off shadowing warnings, add:
       //USE_COMPILE_FLAG -Wno-shadow
   -To trigger the use of a linker flag, add a commented line to the c++ file
    that requires it.  e.g. to include mpi libraries
       //USE_LINK_FLAG -lmpi -lmpi_cxx
   -To trigger the use of a specific compiler, add a commented line to the c++
    file that requires it.  e.g. to use the mpi compiler
       //USE_COMPILER mpic++
  All flags correctly propagate via dependencies, so only add them
  to the files that directly require them.
   -To descend into a directory and compile all source files into shared
    object files, use the shared argument
"""



import sys, os, re



_targetFileTypes = ['.bin', '.exe', '.so', '.dll']
_sourceFileTypes = ['.cpp', '.c++', '.c']
_headerFileTypes = ['.hpp', '.h++', '.h']



###############################################################################
def createMakefile(targets, projectDir, sharedDirs = None):
  """
  (main algorithm)
  create a make file for the specified targets
  """
  # create a MakefileData object
  makefileData = MakefileData()
  
  # set the project directory
  makefileData.projectDir = projectDir
  
  # set the targets
  makefileData.setTargets(targets)
  
  # set the directories of dynamically loaded shared objects
  makefileData.setSharedDirs(sharedDirs)
  
  # get all the file dependencies (and identify the object files)
  makefileData.getFileDeps()
  # get all the object dependencies
  makefileData.getObjDeps()
  # get all the target dependencies
  makefileData.getTargetDeps()
  
  # write the make file
  makefileData.writeMakefile()



###############################################################################
def createSharedDirectoryMakefile(dirName):
  """
  create a makefile assuming all the .cpp files in a directory correspond to
  shared objects
  """
  # get the current directory
  originalDir = os.getcwd()
  try:
    # change to shared directory
    os.chdir(dirName)
    # get the source files
    sourceFiles = [os.path.relpath(f) for f in os.listdir('.') \
                if fileType(f) in _sourceFileTypes]
    # convert to shared object files
    targets = [f.rsplit('.', 1)[0] + '.so' for f in sourceFiles]
    # create a makefile
    createMakefile(targets, originalDir)
    # return to original directory
    os.chdir(originalDir)
  except:
    # return to original directory
    os.chdir(originalDir)
    # re-raise the exception
    raise


###############################################################################
def fileType(fileName):
  return '.' + fileName.split('.')[-1]


  
###############################################################################
def objFromSource(sourceFile, moveObjToBinDir = True):
  """
  derive the object file name from the source file name
  """
  if fileType(sourceFile) not in _sourceFileTypes:
    raise TypeError('%s is not a valid source file' % sourceFile)
  objectFile = os.path.relpath(sourceFile).rsplit('.', 1)[0] + '.o'
  
  if moveObjToBinDir:
    # move object files to bin directory
    (objPath, objBase) = os.path.split(objectFile)
    srcInd = objPath.find('src')
    if srcInd >= 0:
      objectFile = os.path.join(objPath[:srcInd], 'bin', objBase)
    else:
      objectFile = os.path.join('bin', objBase)
  return objectFile



###############################################################################
def sourceFromTarget(targetFile):
  """
  derive the source file name from the target file name
  """
  if fileType(targetFile) in _targetFileTypes:
    baseName = os.path.relpath(targetFile).rsplit('.', 1)[0]
  else:
    baseName = os.path.relpath(targetFile)
  
  potentialFiles = []
  for ext in _sourceFileTypes:
    potentialFiles.append(baseName + ext)
    potentialFiles.append(os.path.join('src', baseName + ext))
  
  for sourceFile in potentialFiles:
    if os.path.isfile(sourceFile):
      return sourceFile
  
  sys.tracebacklimit = 0
  raise TypeError('Could not find a source file corresponding to ' +targetFile)



###############################################################################
def sourceFromHeader(hFile):
  """
  derive the source file name from the header file name
  """
  if fileType(hFile) not in _headerFileTypes:
    raise TypeError('%s is not a valid header file' % hFile)
  
  if hFile.startswith(os.pardir):
    # this file is outside of the project path, don't add its cpp file to the
    # project dependencies
    return None
  
  baseName = os.path.relpath(hFile).rsplit('.', 1)[0]
  potentialFiles = []
  for ext in _sourceFileTypes:
    sourceName = baseName + ext
    potentialFiles.append(sourceName)
    sourceName = sourceName.replace('include' + os.sep, 'src' + os.sep)
    potentialFiles.append(sourceName)
  
  for sourceFile in potentialFiles:
    if os.path.isfile(sourceFile):
      return sourceFile
  
  return None



###############################################################################
class MakefileData:
  """
  class that stores project data and writes Makefile
  """
  def __init__(self):
    
    self.projectDir = None
    
    # for each target, store the objectFile it depends on directly, and
    #    all the object dependencies (sourceFile, depend)
    self.targetDeps = {}
    # for each file, store depends, compiler, linkFlag, compileFlag
    self.fileDeps = {}
    # for each object store sourceFile, depend, compiler, linkFlag, compileFlag
    self.objectDeps = {}
    # store list of shared dirs that should be descended into for additional
    #  makes
    self.sharedDirs = set([])
    
    # move various files to bin directory?  (alternatively keep near cpp files)
    #   -move object files to bin directory?
    self.moveObjToBinDir = True
    #   -move binary target files to bin directory?
    self.moveBinaryTargetToBinDir = True
    #   -move shared object target files to bin directory?
    self.moveSharedToBinDir = True
    # make soft link back from bin directory for shared object files?
    self.linkShared = False


  def setTargets(self, targets):
    """
    set the targets for the make file
    """
    self.targetDeps = {}
    self.objectDeps = {}
    self.extraDeps = {}
    for target in targets:
      # add each target to the make file
      self._newTarget(target)
  
  
  def setSharedDirs(self, sharedDirs):
    if not sharedDirs:
      self.sharedDirs = set([])
    else:
      self.sharedDirs = sharedDirs

  
  def _newFileDep(self, depFile):
    """
    add a new file dependency to the project
    """
    if depFile in self.fileDeps:
      # don't make duplicates
      return
    self.fileDeps[depFile] = { \
      'depends'       : set([]), \
      'object'        : None, \
      'compiler'      : set([]), \
      'link flags'    : set([]), \
      'compile flags' : set([]) \
    }
  
  
  def _newObj(self, objectFile, cppFile):
    """
    add a new object file to the project
    """
    if objectFile in self.objectDeps:
      # don't make duplicates
      return
    self.objectDeps[objectFile] = { \
      'source'        : cppFile, \
      'depends'       : set([cppFile]), \
      'object deps'   : set([]), \
      'compiler'      : set([]), \
      'link flags'    : set([]), \
      'compile flags' : set([]) }
  
  
  def _newTarget(self, targetFile):
    """
    add a new target file (executable or shared object) to the project
    """
    if targetFile in self.targetDeps:
      # don't make duplicates
      return
    
    extension = targetFile.split('.')[-1]
    if extension in _sourceFileTypes or extension in _headerFileTypes:
      raise TypeError('%s is not a valid (executable) target.' % targetFile)
    
    # get a clean path to this file
    targetFile = os.path.relpath(os.path.expanduser(targetFile))
    
    # get the associated source file
    sourceFile = sourceFromTarget(targetFile)
    # add the source file to file deps
    self._newFileDep(sourceFile)
    
    # get the associated object file
    objectFile = objFromSource(sourceFile, self.moveObjToBinDir)
      
    # check if target is a shared object
    sharedObject = targetFile.endswith('.so') or targetFile.endswith('.dll')
    if (sharedObject and self.moveSharedToBinDir) or \
       (not sharedObject and self.moveBinaryTargetToBinDir):
      # keep the actual target in the 'bin' subfolder (link it back later)
      targetFile = os.path.join('bin', targetFile)
    
    # create the target object (a dictionary)
    self.targetDeps[targetFile] = { \
      'source' : sourceFile, \
      'object' : objectFile, \
      'depends'     : set([]), \
      'compiler'    : set([]), \
      'link flags'  : set([]), \
      'shared'      : sharedObject \
    }
  

  def getFileDeps(self):
    """
    get all the file dependencies by looking through source files
    """
    newFileDeps = set( self.fileDeps.keys() )
    
    while newFileDeps:
      # iterate until there are no new file dependencies discovered
      
      # add the new files to the list
      addFiles = newFileDeps.copy()
      # keep track of any new files that are included from the add files
      newFileDeps = set([])
      for depFile in addFiles:
        # loop through each add file and find its dependencies and flags
        
        # look for -files #included in this file 
        #          -any make flags that the file requires
        self.getInclude(depFile, newFileDeps)
        
        if fileType(depFile) in _headerFileTypes:
          # look for an associated source file
          sourceFile = sourceFromHeader(depFile)
          if sourceFile:
            # a source file exists, it's a new dependency
            newFileDeps.add(sourceFile)
            # if the source file is new, add it to the project
            if sourceFile not in self.fileDeps:
              self._newFileDep(sourceFile)
            # get the name of the associated object file
            objectFile = objFromSource(sourceFile, self.moveObjToBinDir)
            # add that info to the depFile dictionary
            self.fileDeps[depFile]['object'] = objectFile
        else:
          # get name of associated object file
          objectFile = objFromSource(depFile, self.moveObjToBinDir)
          # add that info to the depFile dictionary
          self.fileDeps[depFile]['object'] = objectFile
          # if the object file is new add it to the project
          if objectFile not in self.objectDeps:
            self._newObj(objectFile, depFile)

  
  def getInclude(self, depFile, newDepFiles):
    """
    getInclude(self, depFile, newDepFiles)
      look in text depFile  to look for more dependencies (files #included in
      depFile) and various flags (compiler, linkFlags, and compileFlags)
      Any new dependencies are added to the set newDepFiles
    """
    depPath = os.path.dirname(depFile)
    if depPath:
      depPath += os.sep

    includePattern = re.compile(r'#include *"(.*)"')
    includeSet = set([])
    if not os.path.isfile(depFile):
      sys.tracebacklimit = 0
      raise IOError('File %s does not exist' % depFile)
    
    # get the dictionary object
    depItem = self.fileDeps[depFile]

    try:
      with open(depFile, 'r') as fIn:
        for line in fIn:
          foundInclude = includePattern.match(line)
          if foundInclude:
            # a file is #included from depFile
            literalPath = foundInclude.groups()[0]
            newDep = os.path.relpath(os.path.join(depPath, literalPath))
            
            # add to dependencies for depFile
            depItem['depends'].add(newDep)
            
            if newDep not in self.fileDeps:
              # this is a new dependency
              #   -add to list of new dep files
              newDepFiles.add(newDep)
              #   -add to project
              self._newFileDep(newDep)
              if not os.path.isfile(newDep):
                sys.tracebacklimit = 0
                raise RuntimeError( \
                 'File %s (included from %s) does not exist' % \
                 (newDep, depFile) )
  
          linePart = line.partition("//")
          line = linePart[0] + linePart[2]
          linePart = line.partition("/*")
          line = linePart[0] + linePart[2]
          linePart = line.partition("*/")
          line = linePart[0] + linePart[2]
        
          splitList = line.split(None)
          if "USE_LINK_FLAG" in splitList:
            ind = splitList.index("USE_LINK_FLAG")
            linkFlag = splitList[(ind+1):]
            depItem['link flags'].update(linkFlag)
          elif "USE_COMPILE_FLAG" in splitList:
            ind = splitList.index("USE_COMPILE_FLAG")
            compileFlag = splitList[(ind+1):]
            depItem['compile flags'].update(compileFlag)
          elif "USE_COMPILER" in splitList:
            ind = splitList.index("USE_COMPILER")
            compiler = splitList[(ind+1):]
            depItem['compiler'].update(compiler)
    except IOError:
      sys.tracebacklimit = 0
      raise IOError('Error reading %s' % depFile)
  
  
  def getObjDeps(self):
    """
    Get all the object dependencies by looking through fileDeps.
    Propagate back flags
    """
    for (objectFile, objectItem) in self.objectDeps.items():
      # for each object file, obtain its dependencies
      
      newDeps = objectItem['depends']
      while newDeps:
        # iterate until we've found all the dependencies
        
        checkDeps = newDeps.copy()
        newDeps = set([])
        for depFile in checkDeps:
          # loop through the new dep files to check, adding flags and any new
          # dependencies
          
          # get the dependence dictionary object
          depItem = self.fileDeps[depFile]
          
          # update flags
          objectItem['link flags'].update(depItem['link flags'])
          objectItem['compile flags'].update(depItem['compile flags'])
          objectItem['compiler'].update(depItem['compiler'])
          
          # find any new dependencies and add them to newDeps and objItem deps
          unchecked = depItem['depends'] - objectItem['depends']
          newDeps.update(unchecked)
          objectItem['depends'].update(unchecked)
          
          # add any associated object file to object deps
          if depItem['object']:
            objectItem['object deps'].add(depItem['object'])
      
      if objectItem['compiler']:
        # at least one special compiler requested
        if len(objectItem['compiler']) > 1:
          # too many special compilers requested
          raise RuntimeError( objectFile + ' needs multiple compilers: ' + \
                              ' '.join(objectItem['compiler']) )
        # set the compiler to the string for the one requested compiler
        objectItem['compiler'] = objectItem['compiler'].pop()
      else:
        # no special compiler requested, use default
        objectItem['compiler'] = '$(CCC)'
      
  
  
  def getTargetDeps(self):
    """
    Get all the target dependencies by looking through objectDic
    Propagate back link flags and compiler
    """
    for (targetFile, targetItem) in self.targetDeps.items():
      # for each target file obtain its dependencies
      
      # get the associated object dictionary
      objectFile = targetItem['object']
      objectItem = self.objectDeps[objectFile]
      
      # get the dependencies and flags from that dictionary
      targetItem['depends'] = objectItem['object deps']
      targetItem['link flags'] = objectItem['link flags']
      targetItem['compiler'] = objectItem['compiler']
  
  
  def _writeObject(self, f, objectFile, objectItem):
    """
    write the dependencies line, notification line, and compile instruction
    line for this object
    """
    
    # create the dependencies line
    dependLine = objectFile + ': ' + ' '.join(objectItem['depends'])
    # write the dependencies line
    f.write(dependLine + '\n')
    
    # create the compile instruction line
    #  -get the compile flags
    flags = ' '.join(['$(CPPFLAGS)', ' '.join(objectItem['compile flags'])])
    #  -put it all together
    compileLine = '\t@' + objectItem['compiler'] + ' -o ' + objectFile + \
                  ' -c ' + objectItem['source'] + ' ' + flags

    # write notification line
    f.write('\t@echo "   ' + objectItem['compiler'] + ' ' + objectFile + '"\n')

    # write the compile line
    f.write(compileLine + '\n')


  def _writeTarget(self, f, targetFile, targetItem):
    """
    write the dependencies line, notification line, and link instruction line
    for this target
    """
    
    # get the objects that must be linked
    objectFile = targetItem['object']
    if targetItem['shared']:
      # only link the files in the same directory as objectFile
      linkable = [item for item in targetItem['depends'] \
                  if os.path.dirname(item) == os.path.dirname(objectFile)]
      objects = ' '.join(linkable)
    else:
      # link all the dependent objects into an executable
      objects = ' '.join(targetItem['depends'])

    # create the dependencies line
    dependLine = targetFile + ': ' + objects
    # write the dependencies line
    f.write(dependLine + '\n')

    # write notification line
    f.write('\t@echo "   ' + targetItem['compiler'] + ' ' + targetFile + '"\n')
    
    #   -get the link flags
    flags = ' '.join(['$(LDFLAGS)', ' '.join(targetItem['link flags'])])
    if targetItem['shared']:
      # tell compiler to make shared
      flags = '-shared ' + flags
      
    #   -put it all together
    compileLine = '\t@' + targetItem['compiler'] + ' -o ' + targetFile + \
                  ' ' + objects + ' ' + flags
    # write the compile line
    f.write(compileLine + '\n')
  
  
  def _makeTargetLink(self, targetFile):
    """
    make soft link to target if necessary and requested
    """
    sharedObject = self.targetDeps[targetFile]
    if (sharedObject and self.moveSharedToBinDir and self.linkShared) or \
       (not sharedObject and self.moveBinaryTargetToBinDir):
      f.write('\t@if [ ! -f %s ]; then ln -s %s .; fi\n' % \
              (os.path.split(targetFile)[1], targetFile))


  def _removeTargetLink(self, targetFile):
    """
    remove soft link to target if one was created
    """
    sharedObject = self.targetDeps[targetFile]
    if (sharedObject and self.moveSharedToBinDir and self.linkShared) or \
       (not sharedObject and self.moveBinaryTargetToBinDir):
      f.write('\t@rm -f %s\n' % os.path.split(targetFile)[1])
  

  def writeMakefile(self):
    """
    create a Makefile in the working directory
    """
    if os.path.isfile('Makefile.common'):
      makefile_common = 'Makefile.common'
    elif os.path.isfile('../Makefile.common'):
      makefile_common = '../Makefile.common';
    else:
      makefile_common = None
    makefile = 'Makefile'

    with open(makefile, 'w') as f:
      if makefile_common != None:
        f.write('include %s\n' % makefile_common)

      targetsStr = 'TARGET		='
      for targetFile in self.targetDeps:
        targetsStr += ' ' + targetFile
      f.write(targetsStr + '\n')
    
      objectsStr = 'OBJS		='
      for objectFile in self.objectDeps:
        objectsStr += ' ' + objectFile
      f.write(objectsStr + '\n')
      f.write('CPPFLAGS	= $(DEFAULT_CPPFLAGS)\n')
      f.write('LDFLAGS		= $(DEFAULT_LDFLAGS)\n')
      f.write('PROJECTDIR = %s\n' % os.path.relpath(self.projectDir))
      f.write('\n')
      f.write('all:\n')
      f.write('\t@mkdir -p bin\n')
      f.write('\t@echo make $(TARGET)\n')
      f.write('\t@$(MAKE) --silent $(OBJS)\n')
      f.write('\t@$(MAKE) --silent $(TARGET)\n')
      for targetFile in self.targetDeps:
        # make a soft link to target if necessary
        self._makeTargetLink(targetFile)
      f.write('\t@echo Done.\n')
      for sharedDir in self.sharedDirs:
        f.write('\t@make --silent -C %s all\n' % sharedDir)
      f.write('\n')
      
      f.write('# object files:\n')
      for (objectFile, objectItem) in self.objectDeps.items():
        self._writeObject(f, objectFile, objectItem)
      f.write('\n')
      
      f.write('# target files (executable or shared object):\n')
      for (targetFile, targetItem) in self.targetDeps.items():
        self._writeTarget(f, targetFile, targetItem)

      f.write('\n')
      f.write('clean:\n')
      for sharedDir in self.sharedDirs:
        f.write('\t@make --silent -C %s clean\n' % sharedDir)
      f.write('\t@rm -f $(OBJS)\n')
      f.write('\t@rm -f $(TARGET)\n')
      f.write('\t@if [ -d bin ]; then rm -r bin ; fi\n')
      for targetFile in self.targetDeps:
        # remove soft link to target if one was created
        self._removeTargetLink(targetFile)
      #f.write('\n')
      #f.write('install:\n')
      #f.write('\t$(INSTALL) -d $(INSTALLDIR)\n')



###############################################################################
def _parseArguments():
  # get the arguments
  arguments = sys.argv[1:]
  
  if '-shared' in arguments or '--shared' in arguments:
    # all the arguments after '-shared' correspond to directories of
    # dynamically loaded shared objects, the rest are make targets
    try:
      ind = arguments.index('-shared')
    except ValueError:
      ind = arguments.index('--shared')
    targets = set(arguments[0:ind])
    shared = set(arguments[(ind+1):])
  else:
    # there are no shared directories, everything is a make target
    targets = set(arguments)
    shared = set([])
  
  return (targets, shared)

      

###############################################################################
if __name__ == "__main__":
  (targets, shared) = _parseArguments()

  for sharedDir in shared:
    # create make files for all the shared directories
    createSharedDirectoryMakefile(sharedDir) 
  
  # create the main make file
  if targets:
    createMakefile(targets, os.getcwd(), shared)
  
  sys.exit(0)
