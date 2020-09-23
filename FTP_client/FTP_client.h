#pragma once

#include <QtWidgets/QMainWindow>
#include <qfiledialog.h>
#include <qdialog.h>
#include <qdebug.h>
#include <qregexp.h>
#include <QRegExpValidator>
#include <qmessagebox.h>
#include "ui_FTP_client.h"

#include <locale>
#include <codecvt>
#include <qtextcodec.h>

#include <curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif




class FTP_client : public QMainWindow
{
    Q_OBJECT
public:
    FTP_client(QWidget *parent = Q_NULLPTR);
    static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* self);
    long long progress_bar_value;
public slots:    
    void get_file();
    void clear_path() { ui.label_current_path->clear(); file_path = " "; }
    void clear_form() {
        ui.lineEdit_login->clear();
        ui.lineEdit_password->clear();
        ui.port_lineEdit->clear();
        ui.IP_lineEdit->clear();
    }
    void send_file();    
private:
    Ui::FTP_clientClass ui;
    QString file_path;
    QString file_name;
    QString ip_adress; 
    curl_off_t fsize;
    void disabling_buttons(bool);
};
