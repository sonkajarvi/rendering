/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _COLOR_H
#define _COLOR_H

#define COLOR_R(c) (((c) >> 24) & 255)
#define COLOR_G(c) (((c) >> 16) & 255)
#define COLOR_B(c) (((c) >>  8) & 255)
#define COLOR_A(c) (((c)      ) & 255)

#define COLOR_INT_TO_FLOAT(channel) ((float)(channel) / 255.0f)

#endif /* !_COLOR_H */
