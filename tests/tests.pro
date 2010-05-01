TEMPLATE = subdirs

contains(BUILD_FEATURES,coverage) {
SUBDIRS = $$system(ls -1d ut_*/)
} else {
SUBDIRS = $$system(ls -1d ut_*/ ft_*/)
}

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

coverage.target = coverage
coverage.depends = clean
coverage.CONFIG = recursive
QMAKE_EXTRA_TARGETS += coverage

QMAKE_CLEAN += **/*.log.xml **/*.log coverage.out
