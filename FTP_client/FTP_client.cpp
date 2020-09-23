
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "FTP_client.h"

FILE* hd_src = nullptr;

FTP_client::FTP_client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);  
    connect(ui.button_get_file, &QPushButton::clicked, this, &FTP_client::get_file);
    connect(ui.button_clear_path, &QPushButton::clicked, this, &FTP_client::clear_path);
    connect(ui.button_clear_form, &QPushButton::clicked, this, &FTP_client::clear_form);
    connect(ui.button_send_file, &QPushButton::clicked, this, &FTP_client::send_file);
    QString ip_range = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex("^" + ip_range
        + "\\." + ip_range
        + "\\." + ip_range
        + "\\." + ip_range + "$");
    QRegExpValidator* ip_validator = new QRegExpValidator(ipRegex, this);
    ui.IP_lineEdit->setValidator(ip_validator);
}


void FTP_client::get_file()
{
  QFileDialog dialog(this);
  if (dialog.exec()) {     
      QStringList list =  dialog.selectedFiles();
      file_path = list[0];
      file_name = QFileInfo(file_path).fileName();
      ui.label_current_path->setText(file_path);
      struct stat file_info;
      if (stat(std::string(file_path.toLocal8Bit().constData()).c_str(), &file_info)) {
          printf("Couldnt open '%s': %s\n", std::string(file_path.toLocal8Bit().constData()).c_str(), strerror(errno));
      }
      fsize = (curl_off_t)file_info.st_size;
      ui.progressBar->setRange(0,fsize);
      printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);
      ui.label_6->setText(QString::fromLocal8Bit("Размер файла: ") + QString::number(fsize) + QString::fromLocal8Bit(" байт.\n"));
      this->progress_bar_value = 0;
  }

}

void FTP_client::disabling_buttons(bool selector)
{
    switch (selector)
  {
    case true:
    {
        ui.button_clear_form->setDisabled(true);
        ui.button_send_file->setDisabled(true);
        ui.button_get_file->setDisabled(true);
        ui.button_clear_path->setDisabled(true);
        break;
    }
    case false:
    {
        ui.button_clear_form->setDisabled(false);
        ui.button_send_file->setDisabled(false);
        ui.button_get_file->setDisabled(false);
        ui.button_clear_path->setDisabled(false);
        break;
    }
  }


}



void FTP_client::send_file()
{   
    if (std::string(file_path.toLocal8Bit().constData()).c_str()!= " ")
    {
        struct stat file_info;
            CURLcode res;
            CURL* curl;
            struct curl_slist* headerlist = nullptr;

            ip_adress = "ftp://" + ui.IP_lineEdit->text() + "/" + file_name;

            if (stat(std::string(file_path.toLocal8Bit().constData()).c_str(), &file_info)) {
                printf("Couldnt open '%s': %s\n", std::string(file_path.toLocal8Bit().constData()).c_str(), strerror(errno));
            }

        hd_src = fopen(std::string(file_path.toLocal8Bit().constData()).c_str(), "rb");

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl)
        {
            headerlist = curl_slist_append(headerlist, ("RNFR " + file_name).toStdString().c_str());

            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            curl_easy_setopt(curl, CURLOPT_URL, (ip_adress.toStdString().c_str()));
            curl_easy_setopt(curl, CURLOPT_PORT, ui.port_lineEdit->text().toLong());
            curl_easy_setopt(curl, CURLOPT_USERNAME, ui.lineEdit_login->text().toStdString().c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, ui.lineEdit_password->text().toStdString().c_str());
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
            curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
            curl_easy_setopt(curl, CURLOPT_READDATA, this);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                QMessageBox::warning(this, QString::fromLocal8Bit("Внимание!"), curl_easy_strerror(res));

            }
            curl_slist_free_all(headerlist);
            curl_easy_cleanup(curl);
        }
        fclose(hd_src);
        curl_global_cleanup();
        this->progress_bar_value = 0;
        this->ui.label_7->setText(QString::fromLocal8Bit("Доставлено."));
        this->disabling_buttons(false);
    }
}


   size_t FTP_client::read_callback(void* ptr, size_t size, size_t nmemb, void* stream)
  {
    curl_off_t nread;
    FTP_client* ftp_stream = static_cast<FTP_client*>(stream);    
    size_t retcode = fread(ptr, size, nmemb, hd_src);
    nread = (curl_off_t)retcode;
    ftp_stream->progress_bar_value += nread;
    ftp_stream->ui.progressBar->setValue(ftp_stream->progress_bar_value);
    ftp_stream->disabling_buttons(true);
    ftp_stream->ui.label_7->setText(QString::fromLocal8Bit("Идет отправка..."));
    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
        " bytes from file\n", nread);   
    return retcode;
}



























  //, void* not_used
  //  FTP_client* client = static_cast<FTP_client*>(not_used);
//  client->ui.status_window->setText("*** We read " + QString::number(nread) + "bytes from file\n");
  // std::cout << std::string(file_path.toLocal8Bit().constData()).c_str();
  //std::string(file_path.toLocal8Bit().constData()).c_str()