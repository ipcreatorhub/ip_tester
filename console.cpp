/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  console.cpp
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
#include "console.h"

console::console(QObject *parent, QString console_historic_filepath) : QObject(parent)
{
    m_csl_historic_idx = 0;
    //set_console(csl);
    set_console_historic_filepath(console_historic_filepath);
}

console::~console()
{
    QFile file(m_console_historic_filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    for ( int i = 0; i < m_csl_historic.length(); i++ )
        out << (m_csl_historic.at(i));

    file.close();
}

void console::command_trig()
{
    QString cmd = m_console->text();
    m_console->clear();
    m_console->setText(">>");
    cmd_append(cmd);
    emit cmd_avail(cmd);

    return;
}

void console::set_console(QLineEdit *csl)
{
    m_console = csl;
    connect(csl,&QLineEdit::returnPressed,this,&console::command_trig);
    m_console->installEventFilter(this);
    return;

}
void console::set_console_historic_filepath(QString filepath)
{
   m_console_historic_filepath = filepath;
   QFile file(filepath);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return;

   m_csl_historic.clear();
   while (!file.atEnd())
   {
       QByteArray line = file.readLine();
//       if (!line.isEmpty())
//       {
//           line.remove(line.length()-1, 1);
//       }
       m_csl_historic.append(line);
   }
   m_csl_historic_idx = m_csl_historic.length();
   file.close();
   return;
}

void console::cmd_append(QString cmd)
{
    m_csl_historic.append((cmd + "\n"));
    m_csl_historic_idx = m_csl_historic.length();

    return;
}

QString console::next_cmd()
{
    if ((m_csl_historic_idx > m_csl_historic.length()) || (m_csl_historic.isEmpty()))
    {
        return "";
    }
    m_csl_historic_idx++;
    if (m_csl_historic_idx >= m_csl_historic.length())
    {
        m_csl_historic_idx = m_csl_historic.length()-1;
    }
    QString ret = m_csl_historic.at(m_csl_historic_idx);
    if(!ret.isEmpty())
    {
        ret.remove(ret.length()-1, 1);
    }

    if(m_console != nullptr)
    {
        m_console->clear();
        m_console->setText(ret);
    }

    return ret;
}
QString console::prev_cmd()
{
    if ((m_csl_historic_idx > m_csl_historic.length()) || (m_csl_historic.isEmpty()))
    {
        return "";
    }
    m_csl_historic_idx--;
    if (m_csl_historic_idx < 0)
    {
        m_csl_historic_idx = 0;
    }
    QString ret = m_csl_historic.at(m_csl_historic_idx);
    if(!ret.isEmpty())
    {
        ret.remove(ret.length()-1, 1);
    }

    if(m_console != nullptr)
    {
        m_console->clear();
        m_console->setText(ret);
    }

    return ret;
}

bool console::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_console)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);

        switch( event->type() )
        {
        case QEvent::KeyPress:
            if(key_event->key() == Qt::Key_Up)
            {
                prev_cmd();
            }
            else if (key_event->key() == Qt::Key_Down)
            {
                next_cmd();
            }
            break;
        default:
            return QObject::eventFilter(obj, event);
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);

}
