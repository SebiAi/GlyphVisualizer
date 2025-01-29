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

#include "DonationDialog.h"

// Logging
Q_LOGGING_CATEGORY(donationDialog, "DonationDialog")
Q_LOGGING_CATEGORY(donationDialogVerbose, "DonationDialog.Verbose")

DonationDialog::DonationDialog(QWidget* parent, Config* config, bool neverShowAgainOption)
	: QDialog{parent}, config{config}
{
	qCInfo(donationDialog) << "Showing" << (neverShowAgainOption?"with":"without") << "\"never show again\" option.";

	initUi(neverShowAgainOption);
}

void DonationDialog::initUi(bool neverShowAgainOption) {
	// Window title and size
	setWindowTitle(QStringLiteral("Please consider donating"));

	// Layout
	this->layout = new QVBoxLayout(this);
	this->layout->setAlignment(Qt::AlignmentFlag::AlignTop);
	this->layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
	setLayout(layout);

	// Label
	QLabel* label{
		new QLabel{
			QStringLiteral(
				"<h1>Become a <span style=\"color:#1e90ff;\">Supporter</span></h1><br>"
				"<span style=\"font-size:12pt;font-weight:bold;\">Help me maintain projects like GlyphVisualizer and Glyph Tools.</span><br>"
				"<span style=\"font-size:10pt;font-weight:bold;\"><span style=\"color:#228b22;\">Consider donating</span> one-time or supporting me monthly <span style=\"color:red;\">❤️</span></span><br>"
			)
		}
	};
	// label->setWordWrap(true);
	label->setTextFormat(Qt::TextFormat::RichText);
	this->layout->addWidget(label);

	// Configure QDialogButtonBox
	this->buttonBox = new QDialogButtonBox{};
	this->buttonBox->addButton(QStringLiteral("Donate"), QDialogButtonBox::ButtonRole::AcceptRole);
	this->buttonBox->addButton(QStringLiteral("No thanks"), QDialogButtonBox::ButtonRole::RejectRole);
	if (neverShowAgainOption) {
		QPushButton* neverShowAgainButton{new QPushButton{QStringLiteral("No thanks, never show again"), this->buttonBox}};
		connect(neverShowAgainButton, &QPushButton::clicked, this, [=](){
			qCInfo(donationDialog) << "Choose to not show again";
			this->config->setValue(Config::Setting::DonationDialog_DoNotShowAgain_Bool, true);
			reject();
		});
		this->buttonBox->addButton(neverShowAgainButton, QDialogButtonBox::ButtonRole::ResetRole);
	}
	this->layout->addWidget(this->buttonBox);
	connect(this->buttonBox, &QDialogButtonBox::accepted, this, &DonationDialog::accept);
	connect(this->buttonBox, &QDialogButtonBox::rejected, this, &DonationDialog::reject);
}

void DonationDialog::accept() {
	qCInfo(donationDialog).nospace() << "Pressed \"donate\" button";

	// Open donation page
	QDesktopServices::openUrl(QUrl{QStringLiteral(R"(https://ko-fi.com/sebiai)")});

	// Update config - do not show again when user clicked the donate button
	this->config->setValue(Config::Setting::DonationDialog_DoNotShowAgain_Bool, true);

	QDialog::accept();
}
void DonationDialog::reject() {
	qCInfo(donationDialog).nospace() << "Pressed \"no thanks\" button";

	QDialog::reject();
}

void DonationDialog::showEvent(QShowEvent* event) {
	// Update config
	this->config->setValue(Config::Setting::DonationDialog_LastShown_QDateTime, QDateTime::currentDateTimeUtc());

	// Call original
	QDialog::showEvent(event);
}
