#pragma once
#include "exo.h"

exo_cap exo_alloc_page(void);
[[nodiscard]] int exo_unbind_page(exo_cap cap);

void *cap_kalloc(exo_cap *out);
void cap_kfree(exo_cap cap);
