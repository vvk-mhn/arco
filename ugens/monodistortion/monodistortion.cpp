/* monodistortion -- unit generator for arco
 *
 * generated by f2a.py
 */

#include "arcougen.h"
#include "monodistortion.h"

const char *Monodistortion_name = "Monodistortion";

/* O2SM INTERFACE: /arco/monodistortion/new int32 id, int32 chans, int32 snd, int32 gain, int32 tone, int32 volume;
 */
void arco_monodistortion_new(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chans = argv[1]->i;
    int32_t snd = argv[2]->i;
    int32_t gain = argv[3]->i;
    int32_t tone = argv[4]->i;
    int32_t volume = argv[5]->i;
    // end unpack message

    ANY_UGEN_FROM_ID(snd_ugen, snd, "arco_monodistortion_new");
    ANY_UGEN_FROM_ID(gain_ugen, gain, "arco_monodistortion_new");
    ANY_UGEN_FROM_ID(tone_ugen, tone, "arco_monodistortion_new");
    ANY_UGEN_FROM_ID(volume_ugen, volume, "arco_monodistortion_new");

    new Monodistortion(id, chans, snd_ugen, gain_ugen, tone_ugen, volume_ugen);
}


/* O2SM INTERFACE: /arco/monodistortion/repl_snd int32 id, int32 snd_id;
 */
static void arco_monodistortion_repl_snd(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t snd_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_repl_snd");
    ANY_UGEN_FROM_ID(snd, snd_id, "arco_monodistortion_repl_snd");
    monodistortion->repl_snd(snd);
}


/* O2SM INTERFACE: /arco/monodistortion/set_snd int32 id, int32 chan, float val;
 */
static void arco_monodistortion_set_snd (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_set_snd");
    monodistortion->set_snd(chan, val);
}


/* O2SM INTERFACE: /arco/monodistortion/repl_gain int32 id, int32 gain_id;
 */
static void arco_monodistortion_repl_gain(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t gain_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_repl_gain");
    ANY_UGEN_FROM_ID(gain, gain_id, "arco_monodistortion_repl_gain");
    monodistortion->repl_gain(gain);
}


/* O2SM INTERFACE: /arco/monodistortion/set_gain int32 id, int32 chan, float val;
 */
static void arco_monodistortion_set_gain (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_set_gain");
    monodistortion->set_gain(chan, val);
}


/* O2SM INTERFACE: /arco/monodistortion/repl_tone int32 id, int32 tone_id;
 */
static void arco_monodistortion_repl_tone(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t tone_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_repl_tone");
    ANY_UGEN_FROM_ID(tone, tone_id, "arco_monodistortion_repl_tone");
    monodistortion->repl_tone(tone);
}


/* O2SM INTERFACE: /arco/monodistortion/set_tone int32 id, int32 chan, float val;
 */
static void arco_monodistortion_set_tone (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_set_tone");
    monodistortion->set_tone(chan, val);
}


/* O2SM INTERFACE: /arco/monodistortion/repl_volume int32 id, int32 volume_id;
 */
static void arco_monodistortion_repl_volume(O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t volume_id = argv[1]->i;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_repl_volume");
    ANY_UGEN_FROM_ID(volume, volume_id, "arco_monodistortion_repl_volume");
    monodistortion->repl_volume(volume);
}


/* O2SM INTERFACE: /arco/monodistortion/set_volume int32 id, int32 chan, float val;
 */
static void arco_monodistortion_set_volume (O2SM_HANDLER_ARGS)
{
    // begin unpack message (machine-generated):
    int32_t id = argv[0]->i;
    int32_t chan = argv[1]->i;
    float val = argv[2]->f;
    // end unpack message

    UGEN_FROM_ID(Monodistortion, monodistortion, id, "arco_monodistortion_set_volume");
    monodistortion->set_volume(chan, val);
}


static void monodistortion_init()
{
    // O2SM INTERFACE INITIALIZATION: (machine generated)
    o2sm_method_new("/arco/monodistortion/new", "iiiiii",
                    arco_monodistortion_new, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/repl_snd", "ii",
                    arco_monodistortion_repl_snd, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/set_snd", "iif",
                    arco_monodistortion_set_snd, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/repl_gain", "ii",
                    arco_monodistortion_repl_gain, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/set_gain", "iif",
                    arco_monodistortion_set_gain, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/repl_tone", "ii",
                    arco_monodistortion_repl_tone, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/set_tone", "iif",
                    arco_monodistortion_set_tone, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/repl_volume", "ii",
                    arco_monodistortion_repl_volume, NULL, true, true);
    o2sm_method_new("/arco/monodistortion/set_volume", "iif",
                    arco_monodistortion_set_volume, NULL, true, true);
    // END INTERFACE INITIALIZATION

    // class initialization code from faust:
}

Initializer monodistortion_init_obj(monodistortion_init);
