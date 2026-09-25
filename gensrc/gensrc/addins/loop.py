
"""
 Copyright (C) 2006, 2007 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - https://www.quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <https://www.quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
"""

"""Generate source code for loop functions."""

from gensrc.addins import addin
from gensrc.utilities import outputfile
from gensrc.utilities import log
from gensrc.categories import category
from gensrc.configuration import environment

class Loop(addin.Addin):
    """Generate source code for loop functions."""

    #############################################
    # class variables
    #############################################

    # Use a type-erased boost::function to name the type of the object
    # returned by boost::bind.  Naming the concrete boost::bind return type
    # (boost::_bi::bind_t / boost::_mfi::cmfN / boost::_bi::listN) relies on
    # Boost implementation details that changed in newer Boost releases, so
    # boost::function<ReturnType (InputType)> is used instead.  The loop()
    # template only ever invokes the bound object, so type erasure is safe.
    FUNC_BIND = '''\
    typedef     boost::function<%(returnType)s (%(inputType)s)>
                %(functionName)sBind;'''

    FUNC_SIG = '''\
    typedef     %(returnType)s 
                (%(functionType)s::* %(functionSignature)s)(%(signatureArguments)s)%(const)s;'''

    BUF_LOOP = '''
    // %(functionName)s

%(functionBind)s

%(functionSignature)s
'''

    #############################################
    # public interface
    #############################################

    def generate(self, categoryList, enumerationList):
        """Generate source code for Loops."""

        log.Log.instance().logMessage(' begin generating Loops ...')
        for cat in categoryList.categories('*', self.coreCategories_, self.addinCategories_):
            if cat.containsLoopFunction():
                self.generateLoops(cat)
        log.Log.instance().logMessage(' done generating Loops.')

    def generateLoop(self, func):
        """Generate loop typedefs for given function."""
        returnType = self.loopDatatype_.apply(func.returnValue())
        inputType = self.loopDatatype_.apply(func.behavior().loopParamRef_)
        functionBind = Loop.FUNC_BIND % {
            'functionName' : func.name(),
            'inputType' : inputType,
            'returnType' : returnType }
        if func.behavior().functionSignature_:
            if func.const():
                const = ' const'
            else:
                const = ''
            functionSignature = Loop.FUNC_SIG % {
                'const' : const,
                'functionSignature' : func.behavior().functionSignature_,
                'functionType' : func.type(),
                'signatureArguments' : func.parameterList().generate(self.signatureArguments_),
                'returnType' : returnType }
        else:
            functionSignature = ''
            
        return Loop.BUF_LOOP % {
            'functionBind' : functionBind,
            'functionName' : func.name(),
            'functionSignature' : functionSignature }

    def generateLoops(self, cat):
        """Generate type definitions required for source code for loop functions."""
        buf = ''
        for func in cat.functions('*'): 
            if func.loopParameter():
                buf += self.generateLoop(func)
        self.bufferFile_.set({
            'buffer' : buf,
            'namespace' : environment.config().namespaceObjects() })
        fileName = self.rootPath_ + 'loop_' + cat.name() + '.hpp'
        outputfile.OutputFile(self, fileName, self.copyright_, self.bufferFile_)


