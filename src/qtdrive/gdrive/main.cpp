
#include <iostream>
#include <tuple>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>

#include <boost/program_options.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QMetaType>
#include <QDir>

#include "gdrive.h"
#include "tools.h"

using namespace std;
using namespace boost::adaptors;
using namespace boost::program_options;

void show_help(const options_description& desc)
{
    cout << "\nGoogle Drive CLI " << "\n\n";
    cout << desc << "\n";
}

typedef map<QString,
    tuple<
        positional_options_description,
        options_description,
        boost::function<void (gdrive&, const variables_map&)
        >
    >
> commands_t;

commands_t make_commands_desc()
{
    commands_t cmds;
    
    {
        auto& cmd = cmds["ls"];
        
        get<0>(cmd).add("path", 1);

        get<1>(cmd).add_options()
            ("path", value<string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("help", 	"Produce help message")
            ("long,l",  "long format")
            ("raw",     "raw json-output")
        ;
        
        get<2>(cmd) = boost::bind(&gdrive::list, _1, _2);
    }
    
    {
        auto& cmd = cmds["cat"];
        
        get<0>(cmd).add("path", 1);

        get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("format", value<std::string>()->default_value(""), "export in specified format")
            ("help", 	"Produce help message")
        ;
        
        get<2>(cmd) = boost::bind(&gdrive::get, _1, _2);
    }
    
    {
        auto& cmd = cmds["formats"];
        
        get<0>(cmd).add("path", 1);

        get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("help", 	"Produce help message")			
        ;
        
        get<2>(cmd) = boost::bind(&gdrive::formats, _1, _2);
    }

    {
        auto& cmd = cmds["raw"];

        get<0>(cmd).add("path", 1);

        get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("help",    "Produce help message")
        ;

        get<2>(cmd) = boost::bind(&gdrive::raw, _1, _2);
    }
    
    {
        auto& cmd = cmds["get"];

        get<0>(cmd).add("path", 1);
        get<0>(cmd).add("filename", 1);

        get<1>(cmd).add_options()
            ("path", value<std::string>()->default_value(QDir::rootPath().toLocal8Bit().constData()), "path")
            ("format", value<std::string>()->default_value(""), "export in specified format")
            ("filename", value<std::string>()->default_value(""), "filename to store to")
            ("recursevly,r", "download whole directory")
            ("help",    "Produce help message")
        ;

        get<2>(cmd) = boost::bind(&gdrive::get, _1, _2);
    }

    {
        auto& cmd = cmds["put"];

        get<0>(cmd).add("filename", 1);
        get<0>(cmd).add("path", 1);

        get<1>(cmd).add_options()
            ("filename", value<std::string>()->default_value(""), "filename to store")        
            ("path", value<std::string>(), "path to store")
            ("recursive,r", "download whole directory")
            ("help",    "Produce help message")
        ;

        get<2>(cmd) = boost::bind(&gdrive::put, _1, _2);
    }
    
//TODO
//  cd  _unneeded_
//  pwd _unneeded_
//  put
//  mput
// 
//  edit _unneeded_
//  less
//  mkcol mkdir
//  rmcol rm -r
//  delete rm 

//  copy cp
//  move mv
//  lock _unsupported_
//  unlock _unsupported_
//  discover _unsupported_
//  steal _unsupported_
//  showlocks _unsupported_

//  version
//  history
// 
//  checkin ???
//  checkout ???
//  uncheckout ???

//  label title
//  propnames raw field

//  chexec ???
// 
//  propget
//  propdel
//  propset


//  search

    return cmds;
}

static const commands_t c_commands_desc = make_commands_desc();

inline QStringList commands()
{
    QStringList ret;
    BOOST_FOREACH(const QString& cmd, c_commands_desc | map_keys) {
        ret << cmd;
    }
    return ret;
}

typedef vector<string> args_t;

template <typename List>
tuple<args_t, args_t, QString> split_args(const args_t& args, const List& list)
{
    BOOST_FOREACH(const QString& cmd, list) {
        const auto it = find_if(args.begin(), args.end(), [&] (const string& str) {
            return QString::fromLocal8Bit(str.c_str()) == cmd;
        });

        if (it != args.end()) {
            return make_tuple(
                args_t(args.begin(), it + 1),
                args_t(it + 1, args.end()),
                cmd
            );
        }
    }

    return make_tuple(
        args,
        args_t(),
        QString()
    );
}

int main(int argc, char** argv)
{
    const args_t full_args = detail::cmdline(argc, argv).args;

    args_t global_args;
    args_t command_args;
    QString command;

    tie(global_args, command_args, command) = split_args(full_args, c_commands_desc | map_keys);

    string path;
    string filename;
    string format;

    positional_options_description cmd_pd;
    cmd_pd.add("command", 1);

    options_description main_desc("Available options");
    main_desc.add_options()
        ("command", value<args_t>()->required()->multitoken(), commands().join(" | ").toLocal8Bit().constData())
        ("help", 	"Produce help message")
        ("debug,d", "Produce debug messages")
    ;

    try
    {
        variables_map vm;
        parsed_options parsed = command_line_parser(global_args)
            .options(main_desc).positional(cmd_pd).run();

        store(parsed, vm);

        if (vm.count("help"))
        {
            show_help(main_desc);
            return EXIT_SUCCESS;
        }

        notify(vm);

        gdrive cli;
        finilizer f;
        QObject::connect(&cli, SIGNAL(finished(int)), &f, SLOT(exit(int)));

        if (vm.count("command"))
        {
            const auto it = c_commands_desc.find(command);
            if (it == c_commands_desc.end())
                throw runtime_error("no such command");

            const auto& desc = it->second;

            parsed_options parsed = command_line_parser(command_args)
                .positional(get<0>(desc)).options(get<1>(desc)).run();

            store(parsed, vm);

            if (vm.count("help"))
            {
                show_help(get<1>(desc));
                return EXIT_SUCCESS;
            }

            get<2>(desc)(cli, vm);
        }
        else
        {
            show_help(main_desc);
            return EXIT_SUCCESS;
        }


        QCoreApplication app(argc, argv);

        app.setOrganizationName("prog-org-ru-developers");
        app.setApplicationName("gdrive-cli");

        return app.exec();
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        show_help(main_desc);
        return EXIT_FAILURE;
    }
}








