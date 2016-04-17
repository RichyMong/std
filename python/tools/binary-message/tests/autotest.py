import os
import sys
import unittest
import importlib

STD_TESTS = [
    'test_parser',
    'test_response',
    'test_message',
]

class TestFailed(Exception):
    """Test failed."""

def findtests():
    testdir = os.path.dirname(os.path.abspath(__file__))
    names = os.listdir(testdir)
    tests = []
    exclude = set(STD_TESTS)
    for name in names:
        mod, ext = os.path.splitext(name)
        if mod[:5] == "test_" and ext in (".py", "") and mod not in exclude:
            tests.append(mod)
        return STD_TESTS + sorted(tests)

def _run_suite(suite, verbose = True):
    runner = unittest.TextTestRunner(sys.stdout, verbosity=2,
                                     failfast=False)

    result = runner.run(suite)
    if not result.wasSuccessful():
        if len(result.errors) == 1 and not result.failures:
            err = result.errors[0][1]
        elif len(result.failures) == 1 and not result.errors:
            err = result.failures[0][1]
        else:
            err = "multiple errors occurred"
            if not verbose: err += "; run in verbose mode for details"
        raise TestFailed(err)

def run_unittest(*classes):
    """Run tests from unittest.TestCase-derived classes."""
    valid_types = (unittest.TestSuite, unittest.TestCase)
    suite = unittest.TestSuite()
    for cls in classes:
        if isinstance(cls, str):
            if cls in sys.modules:
                suite.addTest(unittest.findTestCases(sys.modules[cls]))
            else:
                raise ValueError("str arguments must be keys in sys.modules")
        elif isinstance(cls, valid_types):
            suite.addTest(cls)
        else:
            suite.addTest(unittest.makeSuite(cls))
    _run_suite(suite)

def runtest(test):
    the_module = importlib.import_module(test)
    test_runner = getattr(the_module, "test_main", None)
    if test_runner is None:
        def test_runner():
            loader = unittest.TestLoader()
            tests = loader.loadTestsFromModule(the_module)
            for error in loader.errors:
                print(error, file=sys.stderr)
            if loader.errors:
                raise TestFailed("errors while loading tests")
            run_unittest(tests)
    test_runner()

if __name__ == '__main__':
    all_tests = findtests()
    for test in all_tests:
        runtest(test)
