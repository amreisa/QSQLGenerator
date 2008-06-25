#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include "ui_AboutDialog.h"

class AboutDlg : 
	public QDialog, 
	public Ui::AboutDialog
{
    Q_OBJECT

public:
    AboutDlg(QWidget *parent = NULL, Qt::WFlags flags = 0);
    ~AboutDlg();

public slots:
	void on_okButton_clicked(bool checked);
};

#endif // ABOUTDLG_H
