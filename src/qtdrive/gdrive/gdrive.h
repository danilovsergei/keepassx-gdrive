
#ifndef GDOCS_GDOCS_H
#define GDOCS_GDOCS_H

#include <memory>

#include <boost/program_options.hpp>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QDir>

#include "../lib/file_info.h"

class gdrive : public QObject {
    Q_OBJECT
public:
    
    explicit gdrive(QObject* parent = 0);
    virtual ~gdrive();
    
Q_SIGNALS:
    void finished(int);

public:
    void list(const boost::program_options::variables_map& vm);
    void formats(const boost::program_options::variables_map& vm);    
    void get(const boost::program_options::variables_map& vm);
    void put(const boost::program_options::variables_map& vm);
    void raw(const boost::program_options::variables_map& vm);
    
private Q_SLOTS:
    void init();
    void refresh_token();

private:
    /// request all items associated with \p path. First element always \b root
	GoogleDrive::FileInfoList request_items(const QString& path);

    GoogleDrive::FileInfoList files_in_folder(const QString& id);
    void get_file(const GoogleDrive::FileInfo& file, const QString& format, QIODevice& output);
    void get_folder(const GoogleDrive::FileInfo& folder, QDir dir = QDir());
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


class FileInfoExplorer
{
public:
    
    FileInfoExplorer(const GoogleDrive::FileInfoList& list);
    
    bool cd(const QString& path);
	bool cd(const QStringList& list);

	
    bool isDir() const;
    bool isFile() const;
    
    QString path() const;

    const GoogleDrive::FileInfo& current() const;

	inline const QString& error() const { return error_; }


private:
    QStringList path_;
    const GoogleDrive::FileInfoList& list_;
	QString error_;
    GoogleDrive::FileInfoList path_list_; 
};




#endif
