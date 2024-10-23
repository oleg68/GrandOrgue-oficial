/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2024 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOPROPERTIESDIALOG_H
#define GOPROPERTIESDIALOG_H

#include <wx/stattext.h>
#include <wx/string.h>

#include "common/GOSimpleDialog.h"

class GOConfig;
class GOOrganController;

class GOPropertiesDialog : public GOSimpleDialog {
private:
  GOOrganController *m_OrganController;

public:
  GOPropertiesDialog(
    GOOrganController *organController, wxWindow *parent, GOConfig &config);
};

#endif // GOPROPERTIESDIALOG_H
