#ifndef PHANTOMTEST_H
#define PHANTOMTEST_H

#include <QObject>

class PhantomTest : public QObject
{
	Q_OBJECT
private slots:
	void testImage_data();
	void testImage();
};

#endif // PHANTOMTEST_H
