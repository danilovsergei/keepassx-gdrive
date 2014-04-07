
#include <iostream>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/adaptors.hpp>

#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QTimer>
#include <QDesktopServices>
#include <QStringList>
#include <QFile>
#include <QDir>

#include <qjson/serializer.h>

#include "../lib/session.h"
#include "../lib/command_oauth2.h"
#include "../lib/command_about.h"
#include "../lib/command_get.h"
#include "../lib/command_file_list.h"
#include "../lib/command_download_file.h"
#include "../lib/command_upload_file.h"

#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <boost/property_tree/json_parser.hpp>
#include "gdrive.h"
#include "map"

using namespace boost::adaptors;

using namespace GoogleDrive;

const QString c_clientId = "1077721689698.apps.googleusercontent.com";
const QString c_clientSecret = "HoJXcAl4JdCDHEohBVAvTW9a";

const QString c_refreshToken = "refreshToken";


struct dir {
    
    static QString simplify(const QString& path) {
        QStringList path_list;
        Q_FOREACH(const QString& token, path.split("/")) {
            if (token.isEmpty()) continue;
            
            path_list << token;
        }
        return "/" + path_list.join("/");
    }
    
    static QString from_native(const QString& path) {
        return simplify(QDir::fromNativeSeparators(path));
    }
    
    static QString to_native(const QString& path) {
        return QDir::toNativeSeparators(simplify(path));
    }

    static bool is_root(const QString& path) {
        return simplify(path) == "/";
    }
    
    static QStringList split(const QString& path) {
        return path.split("/");
    }
};

struct gdrive::Pimpl {
    
    Pimpl()
        : session(&manager)
    {     
        session.setClientId(c_clientId);
        session.setClientSecret(c_clientSecret);
    }

	boost::function<void()> delayed;
    QNetworkAccessManager manager;
    Session session;
    QString error;
};

gdrive::gdrive(QObject* parent)
    : QObject(parent)
    , p_(new Pimpl)
{
    QTimer::singleShot(0, this, SLOT(init()));
}

gdrive::~gdrive()
{
}

void gdrive::init()
{
    QSettings s;
    
    if (!s.value(c_refreshToken).isNull())
    {
        p_->session.setRefreshToken(s.value(c_refreshToken).value<QString>());
    }
    else
    {
        refresh_token();
        s.setValue(c_refreshToken, p_->session.refreshToken());
    }
    p_->delayed();
}

void gdrive::refresh_token()
{
    CommandOAuth2 cmd(&p_->session);
    cmd.setScope(CommandOAuth2::FullAccessScope);

    QDesktopServices::openUrl(cmd.getLoginUrl());

    std::cout << "Enter auth code:" << std::endl;
    std::string code;
    std::cin >> code;

    cmd.requestAccessToken(code.c_str());

    if (!cmd.waitForFinish())
//       throw std::runtime_error(("can't obtain refresh token:" + p_->error).toStdString());
        throw std::runtime_error(qPrintable("can't obtain refresh token:" + p_->error));

}

FileInfoList gdrive::request_items(const QString &path)
{
    FileInfoList result;
    QString root_id;
    
    {
        CommandAbout about_cmd(&p_->session);
        about_cmd.exec();

        if (!about_cmd.waitForFinish())
            throw std::runtime_error(about_cmd.errorString().toLocal8Bit().constData());

        root_id = about_cmd.resultInfo().rootFolderId();
    }
     
    { // obtain root entry itself
        CommandGet get_root(&p_->session);
        get_root.exec(root_id);

        if (!get_root.waitForFinish())
            throw std::runtime_error(get_root.errorString().toLocal8Bit().constData());
        
        result << get_root.resultFileInfo();
    }
    
    if (dir::is_root(path))
    {
        result << files_in_folder(root_id);
    }
    else
    {
        CommandFileList ls_cmd(&p_->session);     
        
        Q_FOREACH(const QString& token, dir::split(path)) {
            if (token.isEmpty()) continue;

            //HACK There is no "or" operator in Google Drive API
            ls_cmd.exec(QString("title = '%1'").arg(token));

            if (!ls_cmd.waitForFinish())
                throw std::runtime_error(ls_cmd.errorString().toLocal8Bit().constData());

            result << ls_cmd.files();
        }        
    }
    
    return result;
}

void gdrive::list(const boost::program_options::variables_map& vm)
{
    assert(!p_->delayed);
    p_->delayed = [&] () {
        const QString native_path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
        const QString path = dir::from_native(native_path);
        const bool long_fmt = vm.count("long");
        const bool raw = vm.count("raw");
        
        FileInfoList files = request_items(path);
        
        FileInfoExplorer e(files);
        if (!e.cd(path))
            throw std::runtime_error(e.error().toLocal8Bit().constData());
        
        if (e.isDir())
        {
            files = files_in_folder(e.current().id());
        }
        else if (e.isFile())
        {
            files = FileInfoList() << e.current();
        }
        
        Q_FOREACH(const GoogleDrive::FileInfo& info, files) {
            if(raw)
            {
                const QByteArray data = QJson::Serializer().serialize(info.rawData());
                std::stringstream ss;
                ss.str(data.constData());

                boost::property_tree::ptree ptree;
                boost::property_tree::read_json( ss, ptree );
        
                boost::property_tree::write_json( std::cout, ptree );
            }
            else if (!long_fmt)
            {
                std::cout << info.title().toLocal8Bit().constData() << std::endl;
            }
            else
            {
                std::cout << 
                    QString("%1%2%3%4%5%6%7 %8")
                        .arg(info.isFolder()    ? "d" : "f")
                        .arg(info.isEditable()  ? "w" : "r")
                        .arg(info.hasLabel(FileInfo::Hidden)    ? "h" : "-")
                        .arg(info.hasLabel(FileInfo::Restricted)? "r" : "-")
                        .arg(info.hasLabel(FileInfo::Starred)   ? "s" : "-")
                        .arg(info.hasLabel(FileInfo::Trashed)   ? "t" : "-")
                        .arg(info.hasLabel(FileInfo::Viewed)    ? "v" : "-")
                        .arg(info.title()).toLocal8Bit().constData()
                << std::endl;
            }
        }
        
        Q_EMIT finished(EXIT_SUCCESS);
    };
}

QUrl extract_download_link(const FileInfo& info, const QString& format)
{
	if (!info.downloadUrl().isEmpty())
	{
		return info.downloadUrl();
	}
	else if (info.exportList().contains(format))
	{
		return info.exportList().value(format);
	}
	else
	{
        QString error_str("invalid export format\nAvailable formats:");
        BOOST_FOREACH(const auto& p, info.exportList().toStdMap()) {
            error_str += "\n\t" + p.first;
        }
		throw std::runtime_error(error_str.toLocal8Bit().constData());
	}
}

QString make_file_name(const FileInfo& info, const QString& format)
{
    if (!info.downloadUrl().isEmpty())
    {
        return info.title();
    }
    else
    {
        QString ext = info.extension(format);
        return info.title() + ((ext.isEmpty()) ? "" : "." + ext);
    }

}

void gdrive::get(const boost::program_options::variables_map& vm)
{
	assert(!p_->delayed);	
	p_->delayed = [&] () {
        const QString native_path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
        const QString path = dir::from_native(native_path);
		const QString format = QString::fromLocal8Bit(vm.at("format").as<std::string>().c_str());
        const QString output = QString::fromLocal8Bit(vm["filename"].as<std::string>().c_str());
        const bool recurse = vm.count("recursevly");
		
		const FileInfoList files = request_items(path);

		FileInfoExplorer e(files);
		if (!e.cd(path))
			throw std::runtime_error(e.error().toLocal8Bit().constData());

		if (e.isFile())
		{
			QFile file(output);

			bool b = (output.isEmpty())
				? file.open(stdout, QIODevice::WriteOnly)
				: file.open(QIODevice::ReadWrite | QIODevice::Truncate);

			if (!b)
				throw std::runtime_error(std::string("Can't open output: ")
					+ ((output.isEmpty()) ? "STDOUT" : output.toLocal8Bit().constData()) );

            get_file(e.current(), format, file);
		}
		else if (e.isDir() && recurse)
        {
            get_folder(e.current());
        }
        else
        {
            throw std::runtime_error("can't download directory");
        }

        Q_EMIT finished(EXIT_SUCCESS);
	};
}

void gdrive::put(const boost::program_options::variables_map& vm)
{
    assert(!p_->delayed);
    p_->delayed = [&] () {
        const QString input = QString::fromLocal8Bit(vm.at("filename").as<std::string>().c_str());
        
        const QString native_path = QString::fromLocal8Bit(vm["path"].as<std::string>().c_str());
        const QString path = dir::from_native(native_path.isEmpty() ? QFileInfo(input).fileName() : native_path);

        const bool recurse = vm.count("recursevly");


        QFile file(input);

        bool b = file.open(QIODevice::ReadOnly);

        if (!b)
            throw std::runtime_error(std::string("Can't open output: ") + input.toLocal8Bit().constData());

        CommandUploadFile upload(&p_->session);

        FileInfo info;
        info.setTitle(input);

        info.rawData()["X-my-field"] = "my-value";
        
        upload.exec(info, &file);

        if (!upload.waitForFinish())
            throw std::runtime_error(upload.errorString().toLocal8Bit().constData());

        Q_EMIT finished(EXIT_SUCCESS);
    };   
}

void gdrive::get_file(const FileInfo& file, const QString& format, QIODevice& output)
{
    CommandDownloadFile dwn_cmd(&p_->session);
    dwn_cmd.exec(extract_download_link(file, format), &output);
    if (!dwn_cmd.waitForFinish())
            throw std::runtime_error(dwn_cmd.errorString().toLocal8Bit().constData());
}

void gdrive::get_folder(const FileInfo& folder, QDir dir)
{
    if (!folder.isFolder())
        throw std::logic_error("entry in not folder");
    
    dir.mkdir(folder.title());
    dir.cd(folder.title());
    
    BOOST_FOREACH(const FileInfo& info, files_in_folder(folder.id())) {
        if (info.isFolder())
        {
            get_folder(info, dir);
        }
        else
        {
            QFile file(dir.filePath(make_file_name(info, "application/pdf")));
            file.open(QIODevice::ReadWrite | QIODevice::Truncate);
             get_file(info, "application/pdf", file);
        }
    }
}

FileInfoList gdrive::files_in_folder(const QString& id)
{
    CommandFileList ls_cmd(&p_->session);
    ls_cmd.execForFolder(id);

    if (!ls_cmd.waitForFinish())
        throw std::runtime_error(ls_cmd.errorString().toLocal8Bit().constData());
    return ls_cmd.files();
}


void gdrive::formats(const boost::program_options::variables_map& vm)
{
	assert(!p_->delayed);	
    p_->delayed = [&] () {
        const QString native_path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
        const QString path = dir::from_native(native_path);  
        const FileInfoList files = request_items(path);

        FileInfoExplorer e(files);
        if (!e.cd(path))
            throw std::runtime_error(e.error().toLocal8Bit().constData());

        Q_FOREACH(const QString& format, e.current().exportList().toStdMap() | map_keys) {
            std::cout << format.toLocal8Bit().constData() << std::endl;
        }
            
        Q_EMIT finished(EXIT_SUCCESS);
    };
}

void gdrive::raw(const boost::program_options::variables_map& vm)
{
    assert(!p_->delayed);
    p_->delayed = [&] () {
        const QString native_path = QString::fromLocal8Bit(vm.at("path").as<std::string>().c_str());
        const QString path = dir::from_native(native_path);
        FileInfoList files = request_items(path);

        FileInfoExplorer e(files);
        if (!e.cd(path))
            throw std::runtime_error(e.error().toLocal8Bit().constData());

        const QByteArray data = QJson::Serializer().serialize(e.current().rawData());
        std::stringstream ss;
        ss.str(data.constData());

        boost::property_tree::ptree ptree;
        boost::property_tree::read_json( ss, ptree );
        
        boost::property_tree::write_json( std::cout, ptree );

        Q_EMIT finished(EXIT_SUCCESS);
    };
}

FileInfoExplorer::FileInfoExplorer(const FileInfoList& list)
    : list_(list)
{
    cd("/");
}

bool FileInfoExplorer::cd(const QStringList& list)
{
    if (list.front().isEmpty())
    {
        cd("/");
    }
    
	Q_FOREACH (const QString& token, list) {
		if (token.isEmpty()) continue;

		const auto finder = [&] (const FileInfo& info) {
			return info.title() == token
				&& (path_list_.isEmpty() || info.parents().contains(path_list_.back().id()) );
		};
		
		const FileInfoList::const_iterator it = std::find_if(list_.begin(), list_.end(), finder);

		if (it == list_.end())
		{
			error_ = "there is no item with name: " + token;
			return false;
		}
		else if (std::find_if(it + 1, list_.end(), finder) != list_.end())
		{
			error_ = "there is more than one item with name: " + token;
			return false;
		}

		path_list_ << *it;
		path_ << token;
	}

	return true;
}

bool FileInfoExplorer::cd(const QString& path)
{
    if (dir::is_root(path))
    {
        path_list_.clear();
        path_.clear();
        
        path_list_ << list_.front();
        path_ << "";
        return true;
    }
    else
    {
        return cd(dir::split(path));
    }
}

QString FileInfoExplorer::path() const
{
    return path_.join("/");
}


bool FileInfoExplorer::isDir() const
{
    return path_list_.back().isFolder();
}

bool FileInfoExplorer::isFile() const
{
    return !path_list_.back().isFolder();
}

const FileInfo& FileInfoExplorer::current() const
{
    return path_list_.back();
}




