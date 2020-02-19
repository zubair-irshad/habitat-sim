import pytest

import examples.settings
import habitat_sim.gfx.LightInfo
import habitat_sim.gfx.LightPositionModel


def test_get_light_setup(sim):
    light_setup = sim.get_light_setup()

    assert len(light_setup) == 0


def test_set_default_light_setup(sim):
    light_setup = sim.get_light_setup()
    assert len(light_setup) == 0

    light_setup.append(LightInfo(position=[1.0, 1.0, 1.0]))

    sim.set_light_setup(light_setup)

    assert sim.get_light_setup() == light_setup


def test_set_custom_light_setup(sim):
    pass
