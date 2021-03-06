/**
 * \file FEMConditionSetupDialog.h
 * 2011/11/07 KR Initial implementation
 */

#ifndef FEMCONDITIONSETUPDIALOG_H
#define FEMCONDITIONSETUPDIALOG_H

#include "ui_FEMConditionSetup.h"
#include "FEMCondition.h"

#include <QDialog>

class QComboBox;
class QPushButton;
class StrictDoubleValidator;

namespace GEOLIB {
	class GeoObject;
}

namespace MeshLib {
	class CFEMesh;
}

/**
 * \brief A dialog window for adding FEM Conditions based
 * on geometrica objects.
 */
class FEMConditionSetupDialog : public QDialog, private Ui_FEMConditionSetup
{
	Q_OBJECT

public:
	/// Constructor for creating a new FEM condition.
	FEMConditionSetupDialog(const std::string &associated_geometry, 
							const GEOLIB::GEOTYPE type, 
							const std::string &geo_name, 
							const GEOLIB::GeoObject* const geo_object, 
							bool  on_points = false,
							QDialog* parent = 0);

	/// Constructor for editing an existing FEM condition.
	FEMConditionSetupDialog(const FEMCondition &cond, QDialog* parent = 0);

	/// Constructor for creating DIRECT FEM conditions on MeshNodes.
	FEMConditionSetupDialog(const std::string &name, const MeshLib::CFEMesh* mesh, QDialog* parent = 0);

	~FEMConditionSetupDialog(void);

private:
	/// Sets layout of the dialog according to properties of the object
	void setupDialog();
	/// Inserts existing values if an existing FEMCondition is being edited
	void setValuesFromCond();

	FEMCondition _cond;
	bool _set_on_points;
	QComboBox* _combobox; //needed for on_points & linear conds
	QPushButton* directButton; // needed for direct conditions
	const MeshLib::CFEMesh* _mesh; // needed for direct conditions
	StrictDoubleValidator* _first_value_validator;

private slots:
	/// Instructions if the OK-Button has been pressed.
	void accept();

	/// Instructions if the Cancel-Button has been pressed.
	void reject();

	void on_condTypeBox_currentIndexChanged(int index);

	void on_disTypeBox_currentIndexChanged(int index);

	void directButton_pressed();

	void addDisValues(std::vector< std::pair<size_t,double> > direct_values);

	void copyCondOnPoints();

	FEMCondition* typeCast(const FEMCondition &cond);

signals:
	void createFEMCondition(std::vector<FEMCondition*>);

};

#endif //FEMCONDITIONSETUPDIALOG_H
