/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  console.h
*
* DESCRIPTION :
*     console class. Manage console commands
*
* CHANGES :
*
* VERSION	DATE		WHO			DETAIL
* 2.0 		11/26/20 	TT      	Initial file creation
*
*/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QEvent>
#include <QKeyEvent>

class console : public QObject
{
    Q_OBJECT
public:
    explicit console(QObject *parent = nullptr, QString console_historic_filepath = "");
    ~console();
private slots:
    void command_trig();
signals:
    void cmd_avail(const QString &cmd);
public slots:
    void set_console(QLineEdit *csl);
    void set_console_historic_filepath(QString filepath);
    void cmd_append(QString cmd);
    QString next_cmd();
    QString prev_cmd();
private:
    QLineEdit *m_console;
    QStringList m_csl_historic;
    QString m_console_historic_filepath;
    int m_csl_historic_idx;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // CONSOLE_H
