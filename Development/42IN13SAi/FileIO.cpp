#include "FileIO.h"

FileIO::FileIO()
{
}


FileIO::~FileIO()
{
}


QString FileIO::LoadFile(QString URI)
{
	QString text;
	QString line;

	QFile file(URI);
	if (!file.exists())
		std::cout << "File doesn't exist" << std::endl;
	
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		while (!stream.atEnd())
		{
			line = stream.readLine();
			text.append(line);
			text.append("\n");
		}
	}
	file.close();

	return text;
}


void FileIO::SaveFile()
{

}
