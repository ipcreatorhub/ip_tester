/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  system_log.h
*
* DESCRIPTION :
*     sustem_log class. Manage the main UI log window
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef SYSTEM_LOG_H
#define SYSTEM_LOG_H

#include <QObject>
#include <QPlainTextEdit>
#include <QList>

#define     NB_LINES_MAX    ( 1000 )

class system_log : public QObject
{
    Q_OBJECT
public:
    explicit system_log(QObject *parent = nullptr);

signals:

public slots:
    // Append a log screen
    void append(QPlainTextEdit *log);
    // Log a text
    void log_this(QString text);
private:
    // List of logs
    QList< QPlainTextEdit * > m_log_list;
    int m_nb_lines;
};

#endif // SYSTEM_LOG_H
