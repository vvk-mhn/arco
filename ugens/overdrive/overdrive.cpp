/* overdrive -- unit generator for arco
 *
 * generated by f2a.py
 */

#include "arcougen.h"
#include "overdrive.h"

const char *Overdrive_name = "Overdrive";

/* O2SM INTERFACE: /arco/overdrive/new int32 id, int32 snd, int32 gain, int32 tone, int32 volume;
 */
void arco_overdrive_new(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t snd = argv[1]->i;
    int32_t gain = argv[2]->i;
    int32_t tone = argv[3]->i;
    int32_t volume = argv[4]->i;
    // end unpack message

    ANY_UGEN_FROM_ID(snd_ugen, snd, "arco_overdrive_new");
    ANY_UGEN_FROM_ID(gain_ugen, gain, "arco_overdrive_new");
    ANY_UGEN_FROM_ID(tone_ugen, tone, "arco_overdrive_new");
    ANY_UGEN_FROM_ID(volume_ugen, volume, "arco_overdrive_new");

    new Overdrive(id, snd_ugen, gain_ugen, tone_ugen, volume_ugen);
}


/* O2SM INTERFACE: /arco/overdrive/repl_snd int32 id, int32 snd_id;
 */
static void arco_overdrive_repl_snd(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t snd_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_repl_snd");
    ANY_UGEN_FROM_ID(snd, snd_id, "arco_overdrive_repl_snd");
    overdrive->repl_snd(snd);
}


/* O2SM INTERFACE: /arco/overdrive/set_snd int32 id, int32 chan, float val;
 */
static void arco_overdrive_set_snd (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_set_snd");
    overdrive->set_snd(chan, val);
}


/* O2SM INTERFACE: /arco/overdrive/repl_gain int32 id, int32 gain_id;
 */
static void arco_overdrive_repl_gain(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t gain_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_repl_gain");
    ANY_UGEN_FROM_ID(gain, gain_id, "arco_overdrive_repl_gain");
    overdrive->repl_gain(gain);
}


/* O2SM INTERFACE: /arco/overdrive/set_gain int32 id, int32 chan, float val;
 */
static void arco_overdrive_set_gain (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_set_gain");
    overdrive->set_gain(chan, val);
}


/* O2SM INTERFACE: /arco/overdrive/repl_tone int32 id, int32 tone_id;
 */
static void arco_overdrive_repl_tone(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t tone_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_repl_tone");
    ANY_UGEN_FROM_ID(tone, tone_id, "arco_overdrive_repl_tone");
    overdrive->repl_tone(tone);
}


/* O2SM INTERFACE: /arco/overdrive/set_tone int32 id, int32 chan, float val;
 */
static void arco_overdrive_set_tone (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_set_tone");
    overdrive->set_tone(chan, val);
}


/* O2SM INTERFACE: /arco/overdrive/repl_volume int32 id, int32 volume_id;
 */
static void arco_overdrive_repl_volume(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t volume_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_repl_volume");
    ANY_UGEN_FROM_ID(volume, volume_id, "arco_overdrive_repl_volume");
    overdrive->repl_volume(volume);
}


/* O2SM INTERFACE: /arco/overdrive/set_volume int32 id, int32 chan, float val;
 */
static void arco_overdrive_set_volume (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Overdrive, overdrive, id, "arco_overdrive_set_volume");
    overdrive->set_volume(chan, val);
}


static void overdrive_init()
{
    // O2SM INTERFACE INITIALIZATION: (machine generated)
    o2sm_method_new("/arco/overdrive/new", "iiiii", arco_overdrive_new,
                    NULL, true, true);
    o2sm_method_new("/arco/overdrive/repl_snd", "ii",
                    arco_overdrive_repl_snd, NULL, true, true);
    o2sm_method_new("/arco/overdrive/set_snd", "iif",
                    arco_overdrive_set_snd, NULL, true, true);
    o2sm_method_new("/arco/overdrive/repl_gain", "ii",
                    arco_overdrive_repl_gain, NULL, true, true);
    o2sm_method_new("/arco/overdrive/set_gain", "iif",
                    arco_overdrive_set_gain, NULL, true, true);
    o2sm_method_new("/arco/overdrive/repl_tone", "ii",
                    arco_overdrive_repl_tone, NULL, true, true);
    o2sm_method_new("/arco/overdrive/set_tone", "iif",
                    arco_overdrive_set_tone, NULL, true, true);
    o2sm_method_new("/arco/overdrive/repl_volume", "ii",
                    arco_overdrive_repl_volume, NULL, true, true);
    o2sm_method_new("/arco/overdrive/set_volume", "iif",
                    arco_overdrive_set_volume, NULL, true, true);
    // END INTERFACE INITIALIZATION

    // class initialization code from faust:
}

Initializer overdrive_init_obj(overdrive_init);
