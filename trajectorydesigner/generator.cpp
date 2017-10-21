#include "generator.h"

extern "C"
{
#include "spiral.h"
#include "radial.h"
#include "cones.h"
#include "variabledensity.h"
}

Generator::Generator(QObject *parent) : QObject(parent)
{

}

Generator::TrajectoryType Generator::trajectoryType()
{
	return m_trajectoryType;
}

void Generator::setTrajectory(TrajectoryType type)
{
	bool changed = m_trajectoryType != type;
	m_trajectoryType = type;

	if(changed && m_autoUpdate)
		update();
}

Trajectory *Generator::trajectory()
{
	return m_trajectory;
}

void Generator::setFieldOfView(float fieldOfView, int axis)
{
	bool changed = m_fieldOfView[axis] != fieldOfView;
	m_fieldOfView[axis] = fieldOfView;

	if(changed && m_autoUpdate)
		update();
}

void Generator::setSpatialResolution(float spatialResolution, int axis)
{
	bool changed = m_spatialResolution[axis] != spatialResolution;
	m_spatialResolution[axis] = spatialResolution;

	if(changed && m_autoUpdate)
		update();
}

void Generator::setReadoutDuration(float readoutDuration)
{
	bool changed = m_readoutDuration != readoutDuration;
	m_readoutDuration = readoutDuration;

	if(changed && m_autoUpdate)
		update();
}

void Generator::setAutoUpdate(bool status)
{
	m_autoUpdate = status;
}

bool Generator::autoUpdate()
{
	return m_autoUpdate;
}

void Generator::setVariableDensity(bool status)
{
	if(status)
	{
		if(!m_variableDensity)
		{
			m_variableDensity = newVariableDensity();
			setToFullySampled(m_variableDensity);
		}
	}
	else
		deleteVariableDensity(&m_variableDensity);
}

VariableDensity *Generator::variableDensity()
{
	return m_variableDensity;
}

void Generator::update()
{
	int previousReadouts = m_trajectory ? m_trajectory->readouts : 0;

	switch(m_trajectoryType)
	{
		case Spiral:
			m_trajectory = generateSpirals(m_variableDensity, m_fieldOfView[0], m_spatialResolution[0], m_readoutDuration, m_samplingInterval, m_readouts, Archimedean, 0, m_fieldOfView[0], m_gradientLimit, m_slewRateLimit);
			break;
		case Radial:
		m_trajectory = generateRadial2D(m_fieldOfView[0], m_fieldOfView[1], EllipticalShape, m_spatialResolution[0], m_spatialResolution[1], EllipticalShape, m_fullProjection, 1, m_gradientLimit, m_slewRateLimit, m_samplingInterval);
			break;
		case Cones3D:
		{
			float fieldOfViewXY = qMax(m_fieldOfView[0], m_fieldOfView[1]);
			float fieldOfView = qMax(fieldOfViewXY, m_fieldOfView[2]);
			if(m_cones)
				deleteCones(m_cones);
			m_cones = generateCones(m_fieldOfView[0], m_fieldOfView[2], m_variableDensity, m_spatialResolution[0], m_spatialResolution[2], 48, 1, NoCompensation, m_readoutDuration, m_samplingInterval, fieldOfView, m_gradientLimit, m_slewRateLimit, StoreAll);
			m_trajectory = &m_cones->trajectory;
		}
			break;
	}

	emit updated(m_trajectory);

	if(previousReadouts!=m_trajectory->readouts)
		emit readoutsChanged(m_trajectory->readouts);
}