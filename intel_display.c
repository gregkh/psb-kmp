/*
 * Copyright © 2006-2007 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *	Eric Anholt <eric@anholt.net>
 */

#include <linux/i2c.h>

bool intel_pipe_has_type (struct drm_crtc *crtc, int type);

typedef struct {
    /* given values */    
    int n;
    int m1, m2;
    int p1, p2;
    /* derived values */
    int	dot;
    int	vco;
    int	m;
    int	p;
} intel_clock_t;

typedef struct {
    int	min, max;
} intel_range_t;

typedef struct {
    int	dot_limit;
    int	p2_slow, p2_fast;
} intel_p2_t;

#define INTEL_P2_NUM		      2

typedef struct {
    intel_range_t   dot, vco, n, m, m1, m2, p, p1;
    intel_p2_t	    p2;
} intel_limit_t;

#define I8XX_DOT_MIN		  25000
#define I8XX_DOT_MAX		 350000
#define I8XX_VCO_MIN		 930000
#define I8XX_VCO_MAX		1400000
#define I8XX_N_MIN		      3
#define I8XX_N_MAX		     16
#define I8XX_M_MIN		     96
#define I8XX_M_MAX		    140
#define I8XX_M1_MIN		     18
#define I8XX_M1_MAX		     26
#define I8XX_M2_MIN		      6
#define I8XX_M2_MAX		     16
#define I8XX_P_MIN		      4
#define I8XX_P_MAX		    128
#define I8XX_P1_MIN		      2
#define I8XX_P1_MAX		     33
#define I8XX_P1_LVDS_MIN	      1
#define I8XX_P1_LVDS_MAX	      6
#define I8XX_P2_SLOW		      4
#define I8XX_P2_FAST		      2
#define I8XX_P2_LVDS_SLOW	      14
#define I8XX_P2_LVDS_FAST	      14 /* No fast option */
#define I8XX_P2_SLOW_LIMIT	 165000

#define I9XX_DOT_MIN		  20000
#define I9XX_DOT_MAX		 400000
#define I9XX_VCO_MIN		1400000
#define I9XX_VCO_MAX		2800000
#define I9XX_N_MIN		      3
#define I9XX_N_MAX		      8
#define I9XX_M_MIN		     70
#define I9XX_M_MAX		    120
#define I9XX_M1_MIN		     10
#define I9XX_M1_MAX		     20
#define I9XX_M2_MIN		      5
#define I9XX_M2_MAX		      9
#define I9XX_P_SDVO_DAC_MIN	      5
#define I9XX_P_SDVO_DAC_MAX	     80
#define I9XX_P_LVDS_MIN		      7
#define I9XX_P_LVDS_MAX		     98
#define I9XX_P1_MIN		      1
#define I9XX_P1_MAX		      8
#define I9XX_P2_SDVO_DAC_SLOW		     10
#define I9XX_P2_SDVO_DAC_FAST		      5
#define I9XX_P2_SDVO_DAC_SLOW_LIMIT	 200000
#define I9XX_P2_LVDS_SLOW		     14
#define I9XX_P2_LVDS_FAST		      7
#define I9XX_P2_LVDS_SLOW_LIMIT		 112000

#define INTEL_LIMIT_I8XX_DVO_DAC    0
#define INTEL_LIMIT_I8XX_LVDS	    1
#define INTEL_LIMIT_I9XX_SDVO_DAC   2
#define INTEL_LIMIT_I9XX_LVDS	    3

static const intel_limit_t intel_limits[] = {
    { /* INTEL_LIMIT_I8XX_DVO_DAC */
        .dot = { .min = I8XX_DOT_MIN,		.max = I8XX_DOT_MAX },
        .vco = { .min = I8XX_VCO_MIN,		.max = I8XX_VCO_MAX },
        .n   = { .min = I8XX_N_MIN,		.max = I8XX_N_MAX },
        .m   = { .min = I8XX_M_MIN,		.max = I8XX_M_MAX },
        .m1  = { .min = I8XX_M1_MIN,		.max = I8XX_M1_MAX },
        .m2  = { .min = I8XX_M2_MIN,		.max = I8XX_M2_MAX },
        .p   = { .min = I8XX_P_MIN,		.max = I8XX_P_MAX },
        .p1  = { .min = I8XX_P1_MIN,		.max = I8XX_P1_MAX },
	.p2  = { .dot_limit = I8XX_P2_SLOW_LIMIT,
		 .p2_slow = I8XX_P2_SLOW,	.p2_fast = I8XX_P2_FAST },
    },
    { /* INTEL_LIMIT_I8XX_LVDS */
        .dot = { .min = I8XX_DOT_MIN,		.max = I8XX_DOT_MAX },
        .vco = { .min = I8XX_VCO_MIN,		.max = I8XX_VCO_MAX },
        .n   = { .min = I8XX_N_MIN,		.max = I8XX_N_MAX },
        .m   = { .min = I8XX_M_MIN,		.max = I8XX_M_MAX },
        .m1  = { .min = I8XX_M1_MIN,		.max = I8XX_M1_MAX },
        .m2  = { .min = I8XX_M2_MIN,		.max = I8XX_M2_MAX },
        .p   = { .min = I8XX_P_MIN,		.max = I8XX_P_MAX },
        .p1  = { .min = I8XX_P1_LVDS_MIN,	.max = I8XX_P1_LVDS_MAX },
	.p2  = { .dot_limit = I8XX_P2_SLOW_LIMIT,
		 .p2_slow = I8XX_P2_LVDS_SLOW,	.p2_fast = I8XX_P2_LVDS_FAST },
    },
    { /* INTEL_LIMIT_I9XX_SDVO_DAC */
        .dot = { .min = I9XX_DOT_MIN,		.max = I9XX_DOT_MAX },
        .vco = { .min = I9XX_VCO_MIN,		.max = I9XX_VCO_MAX },
        .n   = { .min = I9XX_N_MIN,		.max = I9XX_N_MAX },
        .m   = { .min = I9XX_M_MIN,		.max = I9XX_M_MAX },
        .m1  = { .min = I9XX_M1_MIN,		.max = I9XX_M1_MAX },
        .m2  = { .min = I9XX_M2_MIN,		.max = I9XX_M2_MAX },
        .p   = { .min = I9XX_P_SDVO_DAC_MIN,	.max = I9XX_P_SDVO_DAC_MAX },
        .p1  = { .min = I9XX_P1_MIN,		.max = I9XX_P1_MAX },
	.p2  = { .dot_limit = I9XX_P2_SDVO_DAC_SLOW_LIMIT,
		 .p2_slow = I9XX_P2_SDVO_DAC_SLOW,	.p2_fast = I9XX_P2_SDVO_DAC_FAST },
    },
    { /* INTEL_LIMIT_I9XX_LVDS */
        .dot = { .min = I9XX_DOT_MIN,		.max = I9XX_DOT_MAX },
        .vco = { .min = I9XX_VCO_MIN,		.max = I9XX_VCO_MAX },
        .n   = { .min = I9XX_N_MIN,		.max = I9XX_N_MAX },
        .m   = { .min = I9XX_M_MIN,		.max = I9XX_M_MAX },
        .m1  = { .min = I9XX_M1_MIN,		.max = I9XX_M1_MAX },
        .m2  = { .min = I9XX_M2_MIN,		.max = I9XX_M2_MAX },
        .p   = { .min = I9XX_P_LVDS_MIN,	.max = I9XX_P_LVDS_MAX },
        .p1  = { .min = I9XX_P1_MIN,		.max = I9XX_P1_MAX },
	/* The single-channel range is 25-112Mhz, and dual-channel
	 * is 80-224Mhz.  Prefer single channel as much as possible.
	 */
	.p2  = { .dot_limit = I9XX_P2_LVDS_SLOW_LIMIT,
		 .p2_slow = I9XX_P2_LVDS_SLOW,	.p2_fast = I9XX_P2_LVDS_FAST },
    },
};

static const intel_limit_t *intel_limit(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	const intel_limit_t *limit;
	
	if (IS_I9XX(dev)) {
		if (intel_pipe_has_type(crtc, INTEL_OUTPUT_LVDS))
			limit = &intel_limits[INTEL_LIMIT_I9XX_LVDS];
		else
			limit = &intel_limits[INTEL_LIMIT_I9XX_SDVO_DAC];
	} else {
		if (intel_pipe_has_type(crtc, INTEL_OUTPUT_LVDS))
			limit = &intel_limits[INTEL_LIMIT_I8XX_LVDS];
		else
			limit = &intel_limits[INTEL_LIMIT_I8XX_DVO_DAC];
	}
	return limit;
}

/** Derive the pixel clock for the given refclk and divisors for 8xx chips. */

static void i8xx_clock(int refclk, intel_clock_t *clock)
{
	clock->m = 5 * (clock->m1 + 2) + (clock->m2 + 2);
	clock->p = clock->p1 * clock->p2;
	clock->vco = refclk * clock->m / (clock->n + 2);
	clock->dot = clock->vco / clock->p;
}

/** Derive the pixel clock for the given refclk and divisors for 9xx chips. */

static void i9xx_clock(int refclk, intel_clock_t *clock)
{
	clock->m = 5 * (clock->m1 + 2) + (clock->m2 + 2);
	clock->p = clock->p1 * clock->p2;
	clock->vco = refclk * clock->m / (clock->n + 2);
	clock->dot = clock->vco / clock->p;
}

static void intel_clock(struct drm_device *dev, int refclk,
			intel_clock_t *clock)
{
	if (IS_I9XX(dev))
		return i9xx_clock (refclk, clock);
	else
		return i8xx_clock (refclk, clock);
}

/**
 * Returns whether any output on the specified pipe is of the specified type
 */
bool intel_pipe_has_type (struct drm_crtc *crtc, int type)
{
    struct drm_device *dev = crtc->dev;
    struct drm_mode_config *mode_config = &dev->mode_config;
    struct drm_output *l_entry;

    list_for_each_entry(l_entry, &mode_config->output_list, head) {
	    if (l_entry->crtc == crtc) {
		    struct intel_output *intel_output = l_entry->driver_private;
		    if (intel_output->type == type)
			    return true;
	    }
    }
    return false;
}

#define INTELPllInvalid(s)   { /* ErrorF (s) */; return false; }
/**
 * Returns whether the given set of divisors are valid for a given refclk with
 * the given outputs.
 */

static bool intel_PLL_is_valid(struct drm_crtc *crtc, intel_clock_t *clock)
{
	const intel_limit_t *limit = intel_limit (crtc);
	
	if (clock->p1  < limit->p1.min  || limit->p1.max  < clock->p1)
		INTELPllInvalid ("p1 out of range\n");
	if (clock->p   < limit->p.min   || limit->p.max   < clock->p)
		INTELPllInvalid ("p out of range\n");
	if (clock->m2  < limit->m2.min  || limit->m2.max  < clock->m2)
		INTELPllInvalid ("m2 out of range\n");
	if (clock->m1  < limit->m1.min  || limit->m1.max  < clock->m1)
		INTELPllInvalid ("m1 out of range\n");
	if (clock->m1 <= clock->m2)
		INTELPllInvalid ("m1 <= m2\n");
	if (clock->m   < limit->m.min   || limit->m.max   < clock->m)
		INTELPllInvalid ("m out of range\n");
	if (clock->n   < limit->n.min   || limit->n.max   < clock->n)
		INTELPllInvalid ("n out of range\n");
	if (clock->vco < limit->vco.min || limit->vco.max < clock->vco)
		INTELPllInvalid ("vco out of range\n");
	/* XXX: We may need to be checking "Dot clock" depending on the multiplier,
	 * output, etc., rather than just a single range.
	 */
	if (clock->dot < limit->dot.min || limit->dot.max < clock->dot)
		INTELPllInvalid ("dot out of range\n");
	
	return true;
}

/**
 * Returns a set of divisors for the desired target clock with the given
 * refclk, or FALSE.  The returned values represent the clock equation:
 * reflck * (5 * (m1 + 2) + (m2 + 2)) / (n + 2) / p1 / p2.
 */
static bool intel_find_best_PLL(struct drm_crtc *crtc, int target,
				int refclk, intel_clock_t *best_clock)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	intel_clock_t clock;
	const intel_limit_t *limit = intel_limit(crtc);
	int err = target;

	if (IS_I9XX(dev) && intel_pipe_has_type(crtc, INTEL_OUTPUT_LVDS) &&
	    (I915_READ(LVDS) & LVDS_PORT_EN) != 0) {
		/*
		 * For LVDS, if the panel is on, just rely on its current
		 * settings for dual-channel.  We haven't figured out how to
		 * reliably set up different single/dual channel state, if we
		 * even can.
		 */
		if ((I915_READ(LVDS) & LVDS_CLKB_POWER_MASK) ==
		    LVDS_CLKB_POWER_UP)
			clock.p2 = limit->p2.p2_fast;
		else
			clock.p2 = limit->p2.p2_slow;
	} else {
		if (target < limit->p2.dot_limit)
			clock.p2 = limit->p2.p2_slow;
		else
			clock.p2 = limit->p2.p2_fast;
	}
	
	memset (best_clock, 0, sizeof (*best_clock));
	
	for (clock.m1 = limit->m1.min; clock.m1 <= limit->m1.max; clock.m1++) {
		for (clock.m2 = limit->m2.min; clock.m2 < clock.m1 &&
			     clock.m2 <= limit->m2.max; clock.m2++) {
			for (clock.n = limit->n.min; clock.n <= limit->n.max;
			     clock.n++) {
				for (clock.p1 = limit->p1.min;
				     clock.p1 <= limit->p1.max; clock.p1++) {
					int this_err;
					
					intel_clock(dev, refclk, &clock);
					
					if (!intel_PLL_is_valid(crtc, &clock))
						continue;
					
					this_err = abs(clock.dot - target);
					if (this_err < err) {
						*best_clock = clock;
						err = this_err;
					}
				}
			}
		}
	}

	return (err != target);
}

#if 0
void
intel_set_vblank(struct drm_device *dev)
{
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct drm_crtc *crtc;
	struct intel_crtc *intel_crtc;
	int vbl_pipe = 0;

	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		intel_crtc = crtc->driver_private;

		if (crtc->enabled)
			vbl_pipe |= (1<<intel_crtc->pipe);
	}

	dev_priv->vblank_pipe = vbl_pipe;
	i915_enable_interrupt(dev);
}
#endif

void
intel_wait_for_vblank(struct drm_device *dev)
{
	/* Wait for 20ms, i.e. one cycle at 50hz. */
	udelay(20000);
}

void
intel_pipe_set_base(struct drm_crtc *crtc, int x, int y)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	unsigned long Start, Offset;
	int dspbase = (pipe == 0 ? DSPABASE : DSPBBASE);
	int dspsurf = (pipe == 0 ? DSPASURF : DSPBSURF);

	Start = crtc->fb->offset;
	Offset = y * crtc->fb->pitch + x;

	DRM_DEBUG("Writing base %08lX %08lX %d %d\n", Start, Offset, x, y);
	if (IS_I965G(dev)) {
		I915_WRITE(dspbase, Offset);
		I915_READ(dspbase);
		I915_WRITE(dspsurf, Start);
		I915_READ(dspsurf);
	} else {
		I915_WRITE(dspbase, Start + Offset);
		I915_READ(dspbase);
	}
	

	if (!dev_priv->sarea_priv) 
		return;
		
	switch (pipe) {
	case 0:
		dev_priv->sarea_priv->planeA_x = x;
		dev_priv->sarea_priv->planeA_y = y;
		break;
	case 1:
		dev_priv->sarea_priv->planeB_x = x;
		dev_priv->sarea_priv->planeB_y = y;
		break;
	default:
		DRM_ERROR("Can't update pipe %d in SAREA\n", pipe);
		break;
	}
}

/**
 * Sets the power management mode of the pipe and plane.
 *
 * This code should probably grow support for turning the cursor off and back
 * on appropriately at the same time as we're turning the pipe off/on.
 */
static void intel_crtc_dpms(struct drm_crtc *crtc, int mode)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	int dpll_reg = (pipe == 0) ? DPLL_A : DPLL_B;
	int dspcntr_reg = (pipe == 0) ? DSPACNTR : DSPBCNTR;
	int dspbase_reg = (pipe == 0) ? DSPABASE : DSPBBASE;
	int pipeconf_reg = (pipe == 0) ? PIPEACONF : PIPEBCONF;
	u32 temp, temp2;
	bool enabled;

	/* XXX: When our outputs are all unaware of DPMS modes other than off
	 * and on, we should map those modes to DPMSModeOff in the CRTC.
	 */
	switch (mode) {
	case DPMSModeOn:
	case DPMSModeStandby:
	case DPMSModeSuspend:
		/* Enable the DPLL */
		temp = I915_READ(dpll_reg);
		if ((temp & DPLL_VCO_ENABLE) == 0) {
			I915_WRITE(dpll_reg, temp);
			I915_READ(dpll_reg);
			/* Wait for the clocks to stabilize. */
			udelay(150);
			I915_WRITE(dpll_reg, temp | DPLL_VCO_ENABLE);
			I915_READ(dpll_reg);
			/* Wait for the clocks to stabilize. */
			udelay(150);
			I915_WRITE(dpll_reg, temp | DPLL_VCO_ENABLE);
			I915_READ(dpll_reg);
			/* Wait for the clocks to stabilize. */
			udelay(150);
		}
		
		/* Enable the pipe */
		temp = I915_READ(pipeconf_reg);
		if ((temp & PIPEACONF_ENABLE) == 0)
			I915_WRITE(pipeconf_reg, temp | PIPEACONF_ENABLE);

		/* Enable the plane */
		temp = I915_READ(dspcntr_reg);
		if (mode != DPMSModeOn)
			temp2 = temp & ~DISPLAY_PLANE_ENABLE;
		else
			temp2 = temp | DISPLAY_PLANE_ENABLE;

		if (temp != temp2) {
			I915_WRITE(dspcntr_reg, temp2);
			/* Flush the plane changes */
			I915_WRITE(dspbase_reg, I915_READ(dspbase_reg));
		}
		
		intel_crtc_load_lut(crtc);
		
		/* Give the overlay scaler a chance to enable if it's on this pipe */
		//intel_crtc_dpms_video(crtc, TRUE); TODO
	break;
	case DPMSModeOff:
		/* Give the overlay scaler a chance to disable if it's on this pipe */
		//intel_crtc_dpms_video(crtc, FALSE); TODO
		
		/* Disable display plane */
		temp = I915_READ(dspcntr_reg);
		if ((temp & DISPLAY_PLANE_ENABLE) != 0) {
			I915_WRITE(dspcntr_reg, temp & ~DISPLAY_PLANE_ENABLE);
			/* Flush the plane changes */
			I915_WRITE(dspbase_reg, I915_READ(dspbase_reg));
			I915_READ(dspbase_reg);
		}
		
		if (!IS_I9XX(dev)) {
			/* Wait for vblank for the disable to take effect */
			intel_wait_for_vblank(dev);
		}
		
		/* Next, disable display pipes */
		temp = I915_READ(pipeconf_reg);
		if ((temp & PIPEACONF_ENABLE) != 0) {
			I915_WRITE(pipeconf_reg, temp & ~PIPEACONF_ENABLE);
			I915_READ(pipeconf_reg);
		}
		
		/* Wait for vblank for the disable to take effect. */
		intel_wait_for_vblank(dev);
		
		temp = I915_READ(dpll_reg);
		if ((temp & DPLL_VCO_ENABLE) != 0) {
			I915_WRITE(dpll_reg, temp & ~DPLL_VCO_ENABLE);
			I915_READ(dpll_reg);
		}
		
		/* Wait for the clocks to turn off. */
		udelay(150);
		break;
	}
	

	if (!dev_priv->sarea_priv)
		return;

	enabled = crtc->enabled && mode != DPMSModeOff;
	
	switch (pipe) {
	case 0:
		dev_priv->sarea_priv->planeA_w = enabled ? crtc->mode.hdisplay : 0;
		dev_priv->sarea_priv->planeA_h = enabled ? crtc->mode.vdisplay : 0;
		break;
	case 1:
		dev_priv->sarea_priv->planeB_w = enabled ? crtc->mode.hdisplay : 0;
		dev_priv->sarea_priv->planeB_h = enabled ? crtc->mode.vdisplay : 0;
		break;
	default:
		DRM_ERROR("Can't update pipe %d in SAREA\n", pipe);
		break;
	}
}

static bool intel_crtc_lock(struct drm_crtc *crtc)
{
   /* Sync the engine before mode switch */
//   i830WaitSync(crtc->scrn);

#if 0 // TODO def XF86DRI
    return I830DRILock(crtc->scrn);
#else
    return FALSE;
#endif
}

static void intel_crtc_unlock (struct drm_crtc *crtc)
{
#if 0 // TODO def XF86DRI
    I830DRIUnlock (crtc->scrn);
#endif
}

static void intel_crtc_prepare (struct drm_crtc *crtc)
{
	crtc->funcs->dpms(crtc, DPMSModeOff);
}

static void intel_crtc_commit (struct drm_crtc *crtc)
{
	crtc->funcs->dpms(crtc, DPMSModeOn);
}

void intel_output_prepare (struct drm_output *output)
{
	/* lvds has its own version of prepare see intel_lvds_prepare */
	output->funcs->dpms(output, DPMSModeOff);
}

void intel_output_commit (struct drm_output *output)
{
	/* lvds has its own version of commit see intel_lvds_commit */
	output->funcs->dpms(output, DPMSModeOn);
}

static bool intel_crtc_mode_fixup(struct drm_crtc *crtc,
				  struct drm_display_mode *mode,
				  struct drm_display_mode *adjusted_mode)
{
	return true;
}


/** Returns the core display clock speed for i830 - i945 */
int intel_get_core_clock_speed(struct drm_device *dev)
{

	/* Core clock values taken from the published datasheets.
	 * The 830 may go up to 166 Mhz, which we should check.
	 */
	if (IS_I945G(dev))
		return 400000;
	else if (IS_I915G(dev))
		return 333000;
	else if (IS_I945GM(dev) || IS_POULSBO(dev) || IS_845G(dev))
		return 200000;
	else if (IS_I915GM(dev)) {
		u16 gcfgc = 0;

		pci_read_config_word(dev->pdev, I915_GCFGC, &gcfgc);
		
		if (gcfgc & I915_LOW_FREQUENCY_ENABLE)
			return 133000;
		else {
			switch (gcfgc & I915_DISPLAY_CLOCK_MASK) {
			case I915_DISPLAY_CLOCK_333_MHZ:
				return 333000;
			default:
			case I915_DISPLAY_CLOCK_190_200_MHZ:
				return 190000;
			}
		}
	} else if (IS_I865G(dev))
		return 266000;
	else if (IS_I855(dev)) {
#if 0
		PCITAG bridge = pciTag(0, 0, 0); /* This is always the host bridge */
		u16 hpllcc = pciReadWord(bridge, I855_HPLLCC);
		
#endif
		u16 hpllcc = 0;
		/* Assume that the hardware is in the high speed state.  This
		 * should be the default.
		 */
		switch (hpllcc & I855_CLOCK_CONTROL_MASK) {
		case I855_CLOCK_133_200:
		case I855_CLOCK_100_200:
			return 200000;
		case I855_CLOCK_166_250:
			return 250000;
		case I855_CLOCK_100_133:
			return 133000;
		}
	} else /* 852, 830 */
		return 133000;
	
	return 0; /* Silence gcc warning */
}


/**
 * Return the pipe currently connected to the panel fitter,
 * or -1 if the panel fitter is not present or not in use
 */
int intel_panel_fitter_pipe (struct drm_device *dev)
{
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	u32  pfit_control;
    
	/* i830 doesn't have a panel fitter */
	if (IS_I830(dev))
		return -1;
    
	pfit_control = I915_READ(PFIT_CONTROL);
    
	/* See if the panel fitter is in use */
	if ((pfit_control & PFIT_ENABLE) == 0)
		return -1;
	
	/* 965 can place panel fitter on either pipe */
	if (IS_I965G(dev))
		return (pfit_control >> 29) & 0x3;
	
	/* older chips can only use pipe 1 */
	return 1;
}

#define WA_NO_FB_GARBAGE_DISPLAY
#ifdef WA_NO_FB_GARBAGE_DISPLAY
static u32 fp_reg_value[2];
static u32 dpll_reg_value[2];
static u32 dpll_md_reg_value[2];
static u32 dspcntr_reg_value[2];
static u32 pipeconf_reg_value[2];
static u32 htot_reg_value[2];
static u32 hblank_reg_value[2];
static u32 hsync_reg_value[2];
static u32 vtot_reg_value[2];
static u32 vblank_reg_value[2];
static u32 vsync_reg_value[2];
static u32 dspsize_reg_value[2];
static u32 dspstride_reg_value[2];
static u32 dsppos_reg_value[2];
static u32 pipesrc_reg_value[2];

static u32 dspbase_value[2];

static u32 lvds_reg_value[2];
static u32 vgacntrl_reg_value[2];
static u32 pfit_control_reg_value[2];

void intel_crtc_mode_restore(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	int fp_reg = (pipe == 0) ? FPA0 : FPB0;
	int dpll_reg = (pipe == 0) ? DPLL_A : DPLL_B;
	int dpll_md_reg = (intel_crtc->pipe == 0) ? DPLL_A_MD : DPLL_B_MD;
	int dspcntr_reg = (pipe == 0) ? DSPACNTR : DSPBCNTR;
	int pipeconf_reg = (pipe == 0) ? PIPEACONF : PIPEBCONF;
	int htot_reg = (pipe == 0) ? HTOTAL_A : HTOTAL_B;
	int hblank_reg = (pipe == 0) ? HBLANK_A : HBLANK_B;
	int hsync_reg = (pipe == 0) ? HSYNC_A : HSYNC_B;
	int vtot_reg = (pipe == 0) ? VTOTAL_A : VTOTAL_B;
	int vblank_reg = (pipe == 0) ? VBLANK_A : VBLANK_B;
	int vsync_reg = (pipe == 0) ? VSYNC_A : VSYNC_B;
	int dspsize_reg = (pipe == 0) ? DSPASIZE : DSPBSIZE;
	int dspstride_reg = (pipe == 0) ? DSPASTRIDE : DSPBSTRIDE;
	int dsppos_reg = (pipe == 0) ? DSPAPOS : DSPBPOS;
	int pipesrc_reg = (pipe == 0) ? PIPEASRC : PIPEBSRC;
	int dspbase = (pipe == 0 ? DSPABASE : DSPBBASE);

	bool ok, is_sdvo = false, is_dvo = false;
	bool is_crt = false, is_lvds = false, is_tv = false;
	struct drm_mode_config *mode_config = &dev->mode_config;
	struct drm_output *output;

	list_for_each_entry(output, &mode_config->output_list, head) {
		struct intel_output *intel_output = output->driver_private;

		if (output->crtc != crtc)
			continue;

		switch (intel_output->type) {
		case INTEL_OUTPUT_LVDS:
			is_lvds = TRUE;
			break;
		case INTEL_OUTPUT_SDVO:
			is_sdvo = TRUE;
			break;
		case INTEL_OUTPUT_DVO:
			is_dvo = TRUE;
			break;
		case INTEL_OUTPUT_TVOUT:
			is_tv = TRUE;
			break;
		case INTEL_OUTPUT_ANALOG:
			is_crt = TRUE;
			break;
		}
		if(is_lvds && ((lvds_reg_value[pipe] & LVDS_PORT_EN) == 0))
		{
			printk("%s: is_lvds but not the boot display, so return\n",
							__FUNCTION__);
			return;
		}
		output->funcs->prepare(output);
	}

	intel_crtc_prepare(crtc);
	/* Disable the panel fitter if it was on our pipe */
	if (intel_panel_fitter_pipe(dev) == pipe)
		I915_WRITE(PFIT_CONTROL, 0);

	if (dpll_reg_value[pipe] & DPLL_VCO_ENABLE) {
		I915_WRITE(fp_reg, fp_reg_value[pipe]);
		I915_WRITE(dpll_reg, dpll_reg_value[pipe]& ~DPLL_VCO_ENABLE);
		I915_READ(dpll_reg);
		udelay(150);
	}

	/*
	if(is_lvds)
		I915_WRITE(LVDS, lvds_reg_value[pipe]);
	*/
	if (is_lvds) {
		I915_WRITE(LVDS, lvds_reg_value[pipe]);
		I915_READ(LVDS);
	}

	I915_WRITE(fp_reg, fp_reg_value[pipe]);
	I915_WRITE(dpll_reg, dpll_reg_value[pipe]);
	I915_READ(dpll_reg);
	udelay(150);
	//I915_WRITE(dpll_md_reg, dpll_md_reg_value[pipe]);
	I915_WRITE(dpll_reg, dpll_reg_value[pipe]);
	I915_READ(dpll_reg);
	udelay(150);
	I915_WRITE(htot_reg, htot_reg_value[pipe]);
	I915_WRITE(hblank_reg, hblank_reg_value[pipe]);
	I915_WRITE(hsync_reg, hsync_reg_value[pipe]);
	I915_WRITE(vtot_reg, vtot_reg_value[pipe]);
	I915_WRITE(vblank_reg, vblank_reg_value[pipe]);
	I915_WRITE(vsync_reg, vsync_reg_value[pipe]);
	I915_WRITE(dspstride_reg, dspstride_reg_value[pipe]);
	I915_WRITE(dspsize_reg, dspsize_reg_value[pipe]);
	I915_WRITE(dsppos_reg, dsppos_reg_value[pipe]);
	I915_WRITE(pipesrc_reg, pipesrc_reg_value[pipe]);
	I915_WRITE(pipeconf_reg, pipeconf_reg_value[pipe]);
	I915_READ(pipeconf_reg);
	intel_wait_for_vblank(dev);
	I915_WRITE(dspcntr_reg, dspcntr_reg_value[pipe]);
	I915_WRITE(dspbase, dspbase_value[pipe]);
	I915_READ(dspbase);
	I915_WRITE(VGACNTRL, vgacntrl_reg_value[pipe]);
	intel_wait_for_vblank(dev);
	I915_WRITE(PFIT_CONTROL, pfit_control_reg_value[pipe]);
	
	intel_crtc_commit(crtc);
	list_for_each_entry(output, &mode_config->output_list, head) {
		if (output->crtc != crtc)
			continue;

		output->funcs->commit(output);
		//output->funcs->dpms(output, DPMSModeOff);
		//printk("turn off the display first\n");
	}
	return;
}

void intel_crtc_mode_save(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	int fp_reg = (pipe == 0) ? FPA0 : FPB0;
	int dpll_reg = (pipe == 0) ? DPLL_A : DPLL_B;
	int dpll_md_reg = (intel_crtc->pipe == 0) ? DPLL_A_MD : DPLL_B_MD;
	int dspcntr_reg = (pipe == 0) ? DSPACNTR : DSPBCNTR;
	int pipeconf_reg = (pipe == 0) ? PIPEACONF : PIPEBCONF;
	int htot_reg = (pipe == 0) ? HTOTAL_A : HTOTAL_B;
	int hblank_reg = (pipe == 0) ? HBLANK_A : HBLANK_B;
	int hsync_reg = (pipe == 0) ? HSYNC_A : HSYNC_B;
	int vtot_reg = (pipe == 0) ? VTOTAL_A : VTOTAL_B;
	int vblank_reg = (pipe == 0) ? VBLANK_A : VBLANK_B;
	int vsync_reg = (pipe == 0) ? VSYNC_A : VSYNC_B;
	int dspsize_reg = (pipe == 0) ? DSPASIZE : DSPBSIZE;
	int dspstride_reg = (pipe == 0) ? DSPASTRIDE : DSPBSTRIDE;
	int dsppos_reg = (pipe == 0) ? DSPAPOS : DSPBPOS;
	int pipesrc_reg = (pipe == 0) ? PIPEASRC : PIPEBSRC;
	int dspbase = (pipe == 0 ? DSPABASE : DSPBBASE);
	bool ok, is_sdvo = false, is_dvo = false;
	bool is_crt = false, is_lvds = false, is_tv = false;
	struct drm_mode_config *mode_config = &dev->mode_config;
	struct drm_output *output;

	list_for_each_entry(output, &mode_config->output_list, head) {
		struct intel_output *intel_output = output->driver_private;

		if (output->crtc != crtc)
			continue;

		switch (intel_output->type) {
		case INTEL_OUTPUT_LVDS:
			is_lvds = TRUE;
			break;
		case INTEL_OUTPUT_SDVO:
			is_sdvo = TRUE;
			break;
		case INTEL_OUTPUT_DVO:
			is_dvo = TRUE;
			break;
		case INTEL_OUTPUT_TVOUT:
			is_tv = TRUE;
			break;
		case INTEL_OUTPUT_ANALOG:
			is_crt = TRUE;
			break;
		}
	}
	
	fp_reg_value[pipe] = I915_READ(fp_reg);
	dpll_reg_value[pipe] = I915_READ(dpll_reg);
	dpll_md_reg_value[pipe] = I915_READ(dpll_md_reg);
	dspcntr_reg_value[pipe] = I915_READ(dspcntr_reg);
	pipeconf_reg_value[pipe] = I915_READ(pipeconf_reg);
	htot_reg_value[pipe] = I915_READ(htot_reg);
	hblank_reg_value[pipe] = I915_READ(hblank_reg);
	hsync_reg_value[pipe] = I915_READ(hsync_reg);
	vtot_reg_value[pipe] = I915_READ(vtot_reg);
	vblank_reg_value[pipe] = I915_READ(vblank_reg);
	vsync_reg_value[pipe] = I915_READ(vsync_reg);
	dspsize_reg_value[pipe] = I915_READ(dspsize_reg);
	dspstride_reg_value[pipe] = I915_READ(dspstride_reg);
	dsppos_reg_value[pipe] = I915_READ(dsppos_reg);
	pipesrc_reg_value[pipe] = I915_READ(pipesrc_reg);
	dspbase_value[pipe] = I915_READ(dspbase);
	if(is_lvds)
		lvds_reg_value[pipe] = I915_READ(LVDS);
	vgacntrl_reg_value[pipe] = I915_READ(VGACNTRL);
	pfit_control_reg_value[pipe] = I915_READ(PFIT_CONTROL);
}
#endif

static void intel_crtc_mode_set(struct drm_crtc *crtc,
				struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode,
				int x, int y)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	int fp_reg = (pipe == 0) ? FPA0 : FPB0;
	int dpll_reg = (pipe == 0) ? DPLL_A : DPLL_B;
	int dpll_md_reg = (intel_crtc->pipe == 0) ? DPLL_A_MD : DPLL_B_MD;
	int dspcntr_reg = (pipe == 0) ? DSPACNTR : DSPBCNTR;
	int pipeconf_reg = (pipe == 0) ? PIPEACONF : PIPEBCONF;
	int htot_reg = (pipe == 0) ? HTOTAL_A : HTOTAL_B;
	int hblank_reg = (pipe == 0) ? HBLANK_A : HBLANK_B;
	int hsync_reg = (pipe == 0) ? HSYNC_A : HSYNC_B;
	int vtot_reg = (pipe == 0) ? VTOTAL_A : VTOTAL_B;
	int vblank_reg = (pipe == 0) ? VBLANK_A : VBLANK_B;
	int vsync_reg = (pipe == 0) ? VSYNC_A : VSYNC_B;
	int dspsize_reg = (pipe == 0) ? DSPASIZE : DSPBSIZE;
	int dspstride_reg = (pipe == 0) ? DSPASTRIDE : DSPBSTRIDE;
	int dsppos_reg = (pipe == 0) ? DSPAPOS : DSPBPOS;
	int pipesrc_reg = (pipe == 0) ? PIPEASRC : PIPEBSRC;
	int refclk;
	intel_clock_t clock;
	u32 dpll = 0, fp = 0, dspcntr, pipeconf;
	bool ok, is_sdvo = false, is_dvo = false;
	bool is_crt = false, is_lvds = false, is_tv = false;
	struct drm_mode_config *mode_config = &dev->mode_config;
	struct drm_output *output;

	if (!crtc->fb) {
		DRM_ERROR("Can't set mode without attached fb\n");
		return;
	}

	list_for_each_entry(output, &mode_config->output_list, head) {
		struct intel_output *intel_output = output->driver_private;

		if (output->crtc != crtc)
			continue;

		switch (intel_output->type) {
		case INTEL_OUTPUT_LVDS:
			is_lvds = TRUE;
			break;
		case INTEL_OUTPUT_SDVO:
			is_sdvo = TRUE;
			break;
		case INTEL_OUTPUT_DVO:
			is_dvo = TRUE;
			break;
		case INTEL_OUTPUT_TVOUT:
			is_tv = TRUE;
			break;
		case INTEL_OUTPUT_ANALOG:
			is_crt = TRUE;
			break;
		}
	}
	
	if (IS_I9XX(dev)) {
		refclk = 96000;
	} else {
		refclk = 48000;
	}

	ok = intel_find_best_PLL(crtc, adjusted_mode->clock, refclk, &clock);
	if (!ok) {
		DRM_ERROR("Couldn't find PLL settings for mode!\n");
		return;
	}

	fp = clock.n << 16 | clock.m1 << 8 | clock.m2;
	
	dpll = DPLL_VGA_MODE_DIS;
	if (IS_I9XX(dev)) {
		if (is_lvds) {
			dpll |= DPLLB_MODE_LVDS;
			if (IS_POULSBO(dev))
				dpll |= DPLL_DVO_HIGH_SPEED;
		} else
			dpll |= DPLLB_MODE_DAC_SERIAL;
		if (is_sdvo) {
			dpll |= DPLL_DVO_HIGH_SPEED;
			if (IS_I945G(dev) || IS_I945GM(dev) || IS_POULSBO(dev)) {
				int sdvo_pixel_multiply = adjusted_mode->clock / mode->clock;
				dpll |= (sdvo_pixel_multiply - 1) << SDVO_MULTIPLIER_SHIFT_HIRES;
			}
		}
		
		/* compute bitmask from p1 value */
		dpll |= (1 << (clock.p1 - 1)) << 16;
		switch (clock.p2) {
		case 5:
			dpll |= DPLL_DAC_SERIAL_P2_CLOCK_DIV_5;
			break;
		case 7:
			dpll |= DPLLB_LVDS_P2_CLOCK_DIV_7;
			break;
		case 10:
			dpll |= DPLL_DAC_SERIAL_P2_CLOCK_DIV_10;
			break;
		case 14:
			dpll |= DPLLB_LVDS_P2_CLOCK_DIV_14;
			break;
		}
		if (IS_I965G(dev))
			dpll |= (6 << PLL_LOAD_PULSE_PHASE_SHIFT);
	} else {
		if (is_lvds) {
			dpll |= (1 << (clock.p1 - 1)) << DPLL_FPA01_P1_POST_DIV_SHIFT;
		} else {
			if (clock.p1 == 2)
				dpll |= PLL_P1_DIVIDE_BY_TWO;
			else
				dpll |= (clock.p1 - 2) << DPLL_FPA01_P1_POST_DIV_SHIFT;
			if (clock.p2 == 4)
				dpll |= PLL_P2_DIVIDE_BY_4;
		}
	}
	
	if (is_tv) {
		/* XXX: just matching BIOS for now */
/*	dpll |= PLL_REF_INPUT_TVCLKINBC; */
		dpll |= 3;
	}
#if 0
	else if (is_lvds)
		dpll |= PLLB_REF_INPUT_SPREADSPECTRUMIN;
#endif
	else
		dpll |= PLL_REF_INPUT_DREFCLK;
	
	/* setup pipeconf */
	pipeconf = I915_READ(pipeconf_reg);

	/* Set up the display plane register */
	dspcntr = DISPPLANE_GAMMA_ENABLE;

	switch (crtc->fb->bits_per_pixel) {
	case 8:
		dspcntr |= DISPPLANE_8BPP;
		break;
	case 16:
		if (crtc->fb->depth == 15)
			dspcntr |= DISPPLANE_15_16BPP;
		else
			dspcntr |= DISPPLANE_16BPP;
		break;
	case 32:
		dspcntr |= DISPPLANE_32BPP_NO_ALPHA;
		break;
	default:
		DRM_ERROR("Unknown color depth\n");
		return;
	}
	

	if (pipe == 0)
		dspcntr |= DISPPLANE_SEL_PIPE_A;
	else
		dspcntr |= DISPPLANE_SEL_PIPE_B;
	
	if (pipe == 0 && !IS_I965G(dev)) {
		/* Enable pixel doubling when the dot clock is > 90% of the (display)
		 * core speed.
		 *
		 * XXX: No double-wide on 915GM pipe B. Is that the only reason for the
		 * pipe == 0 check?
		 */
		if (mode->clock > intel_get_core_clock_speed(dev) * 9 / 10)
			pipeconf |= PIPEACONF_DOUBLE_WIDE;
		else
			pipeconf &= ~PIPEACONF_DOUBLE_WIDE;
	}

	dspcntr |= DISPLAY_PLANE_ENABLE;
	pipeconf |= PIPEACONF_ENABLE;
	dpll |= DPLL_VCO_ENABLE;

	
	/* Disable the panel fitter if it was on our pipe */
	if (intel_panel_fitter_pipe(dev) == pipe)
		I915_WRITE(PFIT_CONTROL, 0);

	DRM_DEBUG("Mode for pipe %c:\n", pipe == 0 ? 'A' : 'B');
	drm_mode_debug_printmodeline(dev, mode);

        /*psbPrintPll("chosen", &clock);*/
    DRM_DEBUG("clock regs: 0x%08x, 0x%08x,dspntr is 0x%8x, pipeconf is 0x%8x\n", (int)dpll,
	      (int)fp,(int)dspcntr,(int)pipeconf);	
#if 0
	if (!xf86ModesEqual(mode, adjusted_mode)) {
		xf86DrvMsg(pScrn->scrnIndex, X_INFO,
			   "Adjusted mode for pipe %c:\n", pipe == 0 ? 'A' : 'B');
		xf86PrintModeline(pScrn->scrnIndex, mode);
	}
	i830PrintPll("chosen", &clock);
#endif

	if (dpll & DPLL_VCO_ENABLE) {
		I915_WRITE(fp_reg, fp);
		I915_WRITE(dpll_reg, dpll & ~DPLL_VCO_ENABLE);
		I915_READ(dpll_reg);
		udelay(150);
	}
	
	/* The LVDS pin pair needs to be on before the DPLLs are enabled.
	 * This is an exception to the general rule that mode_set doesn't turn
	 * things on.
	 */
	if (is_lvds) {
		u32 lvds = I915_READ(LVDS);
		
		lvds |= LVDS_PORT_EN | LVDS_A0A2_CLKA_POWER_UP | LVDS_PIPEB_SELECT;
		/* Set the B0-B3 data pairs corresponding to whether we're going to
		 * set the DPLLs for dual-channel mode or not.
		 */
		if (clock.p2 == 7)
			lvds |= LVDS_B0B3_POWER_UP | LVDS_CLKB_POWER_UP;
		else
			lvds &= ~(LVDS_B0B3_POWER_UP | LVDS_CLKB_POWER_UP);
		
		/* It would be nice to set 24 vs 18-bit mode (LVDS_A3_POWER_UP)
		 * appropriately here, but we need to look more thoroughly into how
		 * panels behave in the two modes.
		 */
		
		I915_WRITE(LVDS, lvds);
		I915_READ(LVDS);
	}
	
	I915_WRITE(fp_reg, fp);
	I915_WRITE(dpll_reg, dpll);
	I915_READ(dpll_reg);
	/* Wait for the clocks to stabilize. */
	udelay(150);
	
	if (IS_I965G(dev)) {
		int sdvo_pixel_multiply = adjusted_mode->clock / mode->clock;
		I915_WRITE(dpll_md_reg, (0 << DPLL_MD_UDI_DIVIDER_SHIFT) |
			   ((sdvo_pixel_multiply - 1) << DPLL_MD_UDI_MULTIPLIER_SHIFT));
	} else {
		/* write it again -- the BIOS does, after all */
		I915_WRITE(dpll_reg, dpll);
	}
	I915_READ(dpll_reg);
	/* Wait for the clocks to stabilize. */
	udelay(150);
	
	I915_WRITE(htot_reg, (adjusted_mode->crtc_hdisplay - 1) |
		   ((adjusted_mode->crtc_htotal - 1) << 16));
	I915_WRITE(hblank_reg, (adjusted_mode->crtc_hblank_start - 1) |
		   ((adjusted_mode->crtc_hblank_end - 1) << 16));
	I915_WRITE(hsync_reg, (adjusted_mode->crtc_hsync_start - 1) |
		   ((adjusted_mode->crtc_hsync_end - 1) << 16));
	I915_WRITE(vtot_reg, (adjusted_mode->crtc_vdisplay - 1) |
		   ((adjusted_mode->crtc_vtotal - 1) << 16));
	I915_WRITE(vblank_reg, (adjusted_mode->crtc_vblank_start - 1) |
		   ((adjusted_mode->crtc_vblank_end - 1) << 16));
	I915_WRITE(vsync_reg, (adjusted_mode->crtc_vsync_start - 1) |
		   ((adjusted_mode->crtc_vsync_end - 1) << 16));
	I915_WRITE(dspstride_reg, crtc->fb->pitch);
	/* pipesrc and dspsize control the size that is scaled from, which should
	 * always be the user's requested size.
	 */
	I915_WRITE(dspsize_reg, ((mode->vdisplay - 1) << 16) | (mode->hdisplay - 1));
	I915_WRITE(dsppos_reg, 0);
	I915_WRITE(pipesrc_reg, ((mode->hdisplay - 1) << 16) | (mode->vdisplay - 1));
	I915_WRITE(pipeconf_reg, pipeconf);
	I915_READ(pipeconf_reg);
	
	intel_wait_for_vblank(dev);
	
	I915_WRITE(dspcntr_reg, dspcntr);
	
	/* Flush the plane changes */
	intel_pipe_set_base(crtc, x, y);
	
#if 0
	intel_set_vblank(dev);
#endif

	/* Disable the VGA plane that we never use */
	I915_WRITE(VGACNTRL, VGA_DISP_DISABLE);

	intel_wait_for_vblank(dev);    
}

/** Loads the palette/gamma unit for the CRTC with the prepared values */
void intel_crtc_load_lut(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int palreg = (intel_crtc->pipe == 0) ? PALETTE_A : PALETTE_B;
	int i;

	/* The clocks have to be on to load the palette. */
	if (!crtc->enabled)
		return;

	for (i = 0; i < 256; i++) {
		I915_WRITE(palreg + 4 * i,
			   (intel_crtc->lut_r[i] << 16) |
			   (intel_crtc->lut_g[i] << 8) |
			   intel_crtc->lut_b[i]);
	}
}

/** Sets the color ramps on behalf of RandR */
static void intel_crtc_gamma_set(struct drm_crtc *crtc, u16 red, u16 green,
				 u16 blue, int regno)
{
	struct intel_crtc *intel_crtc = crtc->driver_private;
	
	intel_crtc->lut_r[regno] = red >> 8;
	intel_crtc->lut_g[regno] = green >> 8;
	intel_crtc->lut_b[regno] = blue >> 8;
}

/* Returns the clock of the currently programmed mode of the given pipe. */
static int intel_crtc_clock_get(struct drm_device *dev, struct drm_crtc *crtc)
{
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	u32 dpll = I915_READ((pipe == 0) ? DPLL_A : DPLL_B);
	u32 fp;
	intel_clock_t clock;

	if ((dpll & DISPLAY_RATE_SELECT_FPA1) == 0)
		fp = I915_READ((pipe == 0) ? FPA0 : FPB0);
	else
		fp = I915_READ((pipe == 0) ? FPA1 : FPB1);

	clock.m1 = (fp & FP_M1_DIV_MASK) >> FP_M1_DIV_SHIFT;
	clock.m2 = (fp & FP_M2_DIV_MASK) >> FP_M2_DIV_SHIFT;
	clock.n = (fp & FP_N_DIV_MASK) >> FP_N_DIV_SHIFT;
	if (IS_I9XX(dev)) {
		clock.p1 = ffs((dpll & DPLL_FPA01_P1_POST_DIV_MASK) >>
			       DPLL_FPA01_P1_POST_DIV_SHIFT);

		switch (dpll & DPLL_MODE_MASK) {
		case DPLLB_MODE_DAC_SERIAL:
			clock.p2 = dpll & DPLL_DAC_SERIAL_P2_CLOCK_DIV_5 ?
				5 : 10;
			break;
		case DPLLB_MODE_LVDS:
			clock.p2 = dpll & DPLLB_LVDS_P2_CLOCK_DIV_7 ?
				7 : 14;
			break;
		default:
			DRM_DEBUG("Unknown DPLL mode %08x in programmed "
				  "mode\n", (int)(dpll & DPLL_MODE_MASK));
			return 0;
		}

		/* XXX: Handle the 100Mhz refclk */
		i9xx_clock(96000, &clock);
	} else {
		bool is_lvds = (pipe == 1) && (I915_READ(LVDS) & LVDS_PORT_EN);

		if (is_lvds) {
			clock.p1 = ffs((dpll & DPLL_FPA01_P1_POST_DIV_MASK_I830_LVDS) >>
				       DPLL_FPA01_P1_POST_DIV_SHIFT);
			clock.p2 = 14;

			if ((dpll & PLL_REF_INPUT_MASK) ==
			    PLLB_REF_INPUT_SPREADSPECTRUMIN) {
				/* XXX: might not be 66MHz */
				i8xx_clock(66000, &clock);
			} else
				i8xx_clock(48000, &clock);		
		} else {
			if (dpll & PLL_P1_DIVIDE_BY_TWO)
				clock.p1 = 2;
			else {
				clock.p1 = ((dpll & DPLL_FPA01_P1_POST_DIV_MASK_I830) >>
					    DPLL_FPA01_P1_POST_DIV_SHIFT) + 2;
			}
			if (dpll & PLL_P2_DIVIDE_BY_4)
				clock.p2 = 4;
			else
				clock.p2 = 2;

			i8xx_clock(48000, &clock);
		}
	}

	/* XXX: It would be nice to validate the clocks, but we can't reuse
	 * i830PllIsValid() because it relies on the xf86_config output
	 * configuration being accurate, which it isn't necessarily.
	 */

	return clock.dot;
}

/** Returns the currently programmed mode of the given pipe. */
struct drm_display_mode *intel_crtc_mode_get(struct drm_device *dev,
					     struct drm_crtc *crtc)
{
	DRM_DRIVER_PRIVATE_T *dev_priv = dev->dev_private;
	struct intel_crtc *intel_crtc = crtc->driver_private;
	int pipe = intel_crtc->pipe;
	struct drm_display_mode *mode;
	int htot = I915_READ((pipe == 0) ? HTOTAL_A : HTOTAL_B);
	int hsync = I915_READ((pipe == 0) ? HSYNC_A : HSYNC_B);
	int vtot = I915_READ((pipe == 0) ? VTOTAL_A : VTOTAL_B);
	int vsync = I915_READ((pipe == 0) ? VSYNC_A : VSYNC_B);

	mode = kzalloc(sizeof(*mode), GFP_KERNEL);
	if (!mode)
		return NULL;

	mode->clock = intel_crtc_clock_get(dev, crtc);
	mode->hdisplay = (htot & 0xffff) + 1;
	mode->htotal = ((htot & 0xffff0000) >> 16) + 1;
	mode->hsync_start = (hsync & 0xffff) + 1;
	mode->hsync_end = ((hsync & 0xffff0000) >> 16) + 1;
	mode->vdisplay = (vtot & 0xffff) + 1;
	mode->vtotal = ((vtot & 0xffff0000) >> 16) + 1;
	mode->vsync_start = (vsync & 0xffff) + 1;
	mode->vsync_end = ((vsync & 0xffff0000) >> 16) + 1;

	drm_mode_set_name(mode);
	drm_mode_set_crtcinfo(mode, 0);

	return mode;
}

static const struct drm_crtc_funcs intel_crtc_funcs = {
	.dpms = intel_crtc_dpms,
	.lock = intel_crtc_lock,
	.unlock = intel_crtc_unlock,
	.mode_fixup = intel_crtc_mode_fixup,
	.mode_set = intel_crtc_mode_set,
	.gamma_set = intel_crtc_gamma_set,
	.prepare = intel_crtc_prepare,
	.commit = intel_crtc_commit,
};


void intel_crtc_init(struct drm_device *dev, int pipe)
{
	struct drm_crtc *crtc;
	struct intel_crtc *intel_crtc;
	int i;

	crtc = drm_crtc_create(dev, &intel_crtc_funcs);
	if (crtc == NULL)
		return;

	intel_crtc = kzalloc(sizeof(struct intel_crtc), GFP_KERNEL);
	if (intel_crtc == NULL) {
		kfree(crtc);
		return;
	}

	intel_crtc->pipe = pipe;
	for (i = 0; i < 256; i++) {
		intel_crtc->lut_r[i] = i;
		intel_crtc->lut_g[i] = i;
		intel_crtc->lut_b[i] = i;
	}

	crtc->driver_private = intel_crtc;
}

struct drm_crtc *intel_get_crtc_from_pipe(struct drm_device *dev, int pipe)
{
	struct drm_crtc *crtc = NULL;

	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		struct intel_crtc *intel_crtc = crtc->driver_private;
		if (intel_crtc->pipe == pipe)
			break;
	}
	return crtc;
}

int intel_output_clones(struct drm_device *dev, int type_mask)
{
	int index_mask = 0;
	struct drm_output *output;
	int entry = 0;

        list_for_each_entry(output, &dev->mode_config.output_list, head) {
		struct intel_output *intel_output = output->driver_private;
		if (type_mask & (1 << intel_output->type))
			index_mask |= (1 << entry);
		entry++;
	}
	return index_mask;
}


static void intel_setup_outputs(struct drm_device *dev)
{
	struct drm_output *output;

	if (!IS_POULSBO(dev))
		intel_crt_init(dev);

	/* Set up integrated LVDS */
	if (IS_MOBILE(dev) && !IS_I830(dev))
		intel_lvds_init(dev);

	if (IS_I9XX(dev)) {
		intel_sdvo_init(dev, SDVOB);
		intel_sdvo_init(dev, SDVOC);
	}

	list_for_each_entry(output, &dev->mode_config.output_list, head) {
		struct intel_output *intel_output = output->driver_private;
		int crtc_mask = 0, clone_mask = 0;
		
		/* valid crtcs */
		switch(intel_output->type) {
		case INTEL_OUTPUT_DVO:
		case INTEL_OUTPUT_SDVO:
			crtc_mask = ((1 << 0)|
				     (1 << 1));
			clone_mask = ((1 << INTEL_OUTPUT_ANALOG) |
				      (1 << INTEL_OUTPUT_DVO) |
				      (1 << INTEL_OUTPUT_SDVO));
			break;
		case INTEL_OUTPUT_ANALOG:
			crtc_mask = ((1 << 0)|
				     (1 << 1));
			clone_mask = ((1 << INTEL_OUTPUT_ANALOG) |
				      (1 << INTEL_OUTPUT_DVO) |
				      (1 << INTEL_OUTPUT_SDVO));
			break;
		case INTEL_OUTPUT_LVDS:
			crtc_mask = (1 << 1);
			clone_mask = (1 << INTEL_OUTPUT_LVDS);
			break;
		case INTEL_OUTPUT_TVOUT:
			crtc_mask = ((1 << 0) |
				     (1 << 1));
			clone_mask = (1 << INTEL_OUTPUT_TVOUT);
			break;
		}
		output->possible_crtcs = crtc_mask;
		output->possible_clones = intel_output_clones(dev, clone_mask);
	}
}

void intel_modeset_init(struct drm_device *dev)
{
	int num_pipe;
	int i;

	drm_mode_config_init(dev);

	dev->mode_config.min_width = 0;
	dev->mode_config.min_height = 0;

	dev->mode_config.max_width = 4096;
	dev->mode_config.max_height = 4096;

	/* set memory base */
	if (IS_I9XX(dev))
		dev->mode_config.fb_base = pci_resource_start(dev->pdev, 2);
	else
		dev->mode_config.fb_base = pci_resource_start(dev->pdev, 0);

	if (IS_MOBILE(dev) || IS_I9XX(dev))
		num_pipe = 2;
	else
		num_pipe = 1;
	DRM_DEBUG("%d display pipe%s available.\n",
		  num_pipe, num_pipe > 1 ? "s" : "");

	for (i = 0; i < num_pipe; i++) {
		intel_crtc_init(dev, i);
	}

	intel_setup_outputs(dev);

	//drm_initial_config(dev, false);
}

void intel_modeset_cleanup(struct drm_device *dev)
{
	drm_mode_config_cleanup(dev);
}
