/*
This file is part of the GlyphVisualizer project, a Glyph composition
player that plays Glyph compositions from Nothing phones.
Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef GV_DONATIONDIALOG_H
#define GV_DONATIONDIALOG_H

#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QStringLiteral>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

#include "Config.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(donationDialog)
Q_DECLARE_LOGGING_CATEGORY(donationDialogVerbose)

class DonationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DonationDialog(QWidget* parent, Config* config, bool neverShowAgainOption);

public slots:
    virtual void accept() override;
    virtual void reject() override;

protected:
    virtual void showEvent(QShowEvent* event) override;

private:
    Config* config;

    QVBoxLayout* layout;

    // QCheckBox* doNotShowAgainCheckBox;
    QDialogButtonBox* buttonBox;

    void initUi(bool neverShowAgainOption);
};

#endif // GV_DONATIONDIALOG_H
