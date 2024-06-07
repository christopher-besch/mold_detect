#pragma once

void perform_measurement();

// true iff the systems signals that the atmosphere is bad
// (i.e. the atmosphere led is on)
int is_atmosphere_bad();
