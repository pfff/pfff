set(sources AssertException Checks CurrentTest DeferredTestReporter DeferredTestResult
            MemoryOutStream ReportAssert Test TestDetails TestList TestReporter TestReporterStdout
            TestResults TestRunner TimeConstraint XmlTestReporter)

if(WIN32)
	list(APPEND sources Win32/TimeHelpers)
else()
	list(APPEND sources Posix/TimeHelpers Posix/SignalTranslator)
endif()

add_library(UnitTest++ STATIC ${sources})
