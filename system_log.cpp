/***********************************************************************
* COMPANY :   IPCREATOR
* AUTHOR  :   Tuan TRUONG
*
* FILENAME :  system_log.cpp
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
#include "system_log.h"

system_log::system_log(QObject *parent) : QObject(parent)
{
    m_nb_lines = 0;
}


/*!
 * \brief system_log::append. Append to the log screen
 * \param log
 */
void system_log::append(QPlainTextEdit *log)
{
    m_log_list.append(log);

    return;
}

/*!
 * \brief system_log::log_this. Log a string
 * \param text. Text to log
 */
void system_log::log_this(QString text)
{
    for(int i = 0; i < m_log_list.length(); i++)
    {
        m_log_list[i]->appendPlainText(text);
    }
    m_nb_lines++;
    if(m_nb_lines == NB_LINES_MAX)
    {
        m_nb_lines = 0;
        for(int i = 0; i < m_log_list.length(); i++)
        {
            m_log_list[i]->clear();
        }
    }


    return;
}
