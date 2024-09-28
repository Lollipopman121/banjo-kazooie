#include <ultra64.h>
#include "functions.h"
#include "variables.h"

void TTC_Nipper_updateFunc(Actor *this);
Actor *TTC_Nipper_animFunc(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx);


/* .data */
ActorAnimationInfo TTC_NIPPER_ANIMATIONS[8] = {
    {0x00, 0.0f},
    {ASSET_C0_ANIM_NIPPER_IDLE, 2.0f},
    {ASSET_BD_ANIM_NIPPER_VULNERABLE, 1.5f},
    {ASSET_BF_ANIM_NIPPER_ATTACK, 1.2f},
    {ASSET_BE_ANIM_NIPPER_OW, 1.3f},
    {ASSET_C0_ANIM_NIPPER_IDLE, 2.0f},
    {ASSET_133_ANIM_NIPPER_DIE, 3.0f},
    {0x00, 0.0f}
};

ActorInfo TTC_NIPPER_ACTOR = { 
    MARKER_A5_NIPPER, ACTOR_117_NIPPER, ASSET_3D5_MODEL_NIPPER,
    0x1, TTC_NIPPER_ANIMATIONS,
    TTC_Nipper_updateFunc, func_80326224, TTC_Nipper_animFunc,
    0, 0x299, 10.0f, 0
};

enum ttc_nipper_states_e {
    TTC_NIPPER_STATE_1_UNKNOWN = 1, // L80388578
    TTC_NIPPER_STATE_2_UNKNOWN = 2, // L8038872C
    TTC_NIPPER_STATE_3_UNKNOWN = 3, // L803887B4
    TTC_NIPPER_STATE_DIEING = 4,    // L80388910
    TTC_NIPPER_STATE_SPAWNED = 5,   // L80388938
    TTC_NIPPER_STATE_DEAD = 6,      // L803889A8
    TTC_NIPPER_STATE_7_UNKNOWN = 7  // already dead? L80388A20
};

/* .code */
void TTC_Nipper_drawParticlesAtPosition(f32 *position, s32 count) {
    static s32 D_8038C5A4[3] = {180, 180, 180};
    static ParticleScaleAndLifetimeRanges D_8038C5B0 = {
        {0.1f, 0.5f},
        {1.4f, 2.8f},
        {0.0f, 0.01f},
        {0.5f, 1.4f},
        0.0f, 0.01f
    };
    ParticleEmitter *pCtrl;

    pCtrl = partEmitMgr_newEmitter(count);
    pCtrl = pCtrl;
    particleEmitter_setSprite(pCtrl, ASSET_700_SPRITE_DUST);
    particleEmitter_setRGB(pCtrl, D_8038C5A4);
    particleEmitter_setStartingFrameRange(pCtrl, 0, 7);
    particleEmitter_setPosition(pCtrl, position);
    particleEmitter_setParticleSpawnPositionRange(pCtrl,
        -10.0f, -5.0f, -10.0f,
         10.0f, 10.0f,  10.0f
    );
    particleEmitter_setParticleVelocityRange(pCtrl,
        -40.0f, 10.0f, -40.0f,
         40.0f, 40.0f,  40.0f
    );
    particleEmitter_setScaleAndLifetimeRanges(pCtrl, &D_8038C5B0);
    particleEmitter_emitN(pCtrl, count);
}

Actor *TTC_Nipper_animFunc(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Actor *this;
    
    this = marker_getActor(marker);
    func_8033A45C(3, (this->state == 7)? 0 : 1);
    return actor_draw(marker, gfx, mtx, vtx);
}

void TTC_Nipper_setAnimationDuration(Actor *this) {
    subaddie_set_state_looped(this, TTC_NIPPER_STATE_3_UNKNOWN);
    this->unk1C[0] = 0.0f;
    switch((s32)this->lifetime_value){
        case 120:
            animctrl_setDuration(this->animctrl, 1.2f);
            break;

        case 80:
            animctrl_setDuration(this->animctrl, 1.05f);
            break;

        case 40:
            animctrl_setDuration(this->animctrl, 0.9f);
            break;
    }
}

void TTC_Nipper_spawnedShowTextCallback(ActorMarker *caller, enum asset_e text_id, s32 arg2){
    Actor *this;
    this = marker_getActor(caller);
    TTC_Nipper_setAnimationDuration(this);
    ncStaticCamera_exit();
    comusic_8025AB44(COMUSIC_12_TTC_NIPPER, -1, 300);
}

void TTC_Nipper_playDeathAnimation(Actor *this) {
    s32 i;

    subaddie_set_state_with_direction(this, TTC_NIPPER_STATE_DIEING, 0.01f, 1);
    actor_playAnimationOnce(this);
    for(i = 0; i < 3; i ++){
        FUNC_8030E8B4(SFX_79_TICKER_DEATH, 0.5f, 17000, this->position, 1500, 3000);
    };
}

// if player too far away -> false
// otherwise return whether sp2C is within -35 and 35?
bool func_80388088(Actor *this){
    f32 sp2C;
    f32 sp20[3];
    bool out;

    sp2C = this->yaw - func_80329784(this);
    player_getPosition(sp20);
    if(sp20[0] < -5680.0f){
        return FALSE;
    }

    return BOOL(-35.0f < sp2C && sp2C < 35.0f);
}

bool TTC_Nipper_shouldShowActor(Actor *this){
    return BOOL(func_80329530(this, 1300) && func_80388088(this));
}

void TTC_Nipper_dieFunc(ActorMarker *this_marker, ActorMarker *other_marker) {
    Actor *this;
    s32 i;

    this = marker_getActor(this_marker);
    func_8032B4DC(this, other_marker, 7);

    if (this->lifetime_value == 40.0f) {
        subaddie_set_state_with_direction(this, TTC_NIPPER_STATE_DEAD, 0.01f, 1);
        actor_playAnimationOnce(this);
        for(i = 0; i < 3; i++){
            FUNC_8030E8B4(SFX_78_EAGLECRY, 0.7f, 20000, this->position, 1500, 3000);
        };
        comusic_8025AB44(COMUSIC_12_TTC_NIPPER, 0, 300);
        func_8025AABC(COMUSIC_12_TTC_NIPPER);
        func_8032BB88(this, -1, 300);
        func_802BAFE4(0x1C);
        return;
    }
    
    if (this->lifetime_value == 80.0f) {
        TTC_Nipper_playDeathAnimation(this);
        this->lifetime_value = 40.0f;
        return;
    }

    TTC_Nipper_playDeathAnimation(this);
    this->lifetime_value = 80.0f;
    gcdialog_showText(ASSET_A10_TEXT_TTC_NIPPER_HURT, 4, NULL, NULL, NULL, NULL);
    return;
}

bool TTC_Nipper_determineMarkeId(ActorMarker * this_marker, ActorMarker * other_marker){
    if(this_marker->unk40_31 == 1){
        this_marker->id = 0x16C;
    }
    else{
        this_marker->id = MARKER_A5_NIPPER;
    }
    return TRUE;
}

void TTC_Nipper_ow2Func(ActorMarker * this_marker, ActorMarker *other_marker){
    Actor *this;

    if(other_marker->id == 1){
        this = marker_getActor(this_marker);
        if( !mapSpecificFlags_get(7)
            && this->has_met_before
            && gcdialog_showText(0xa0f, 0, NULL, NULL, NULL, NULL)
        ){
            mapSpecificFlags_set(7, TRUE);
        }
    }
}

void TTC_Nipper_owFunc(ActorMarker * this_marker, ActorMarker *other_marker){
    Actor *this = marker_getActor(this_marker);
    if( !this->unk138_23
        && this->has_met_before
        && gcdialog_showText(0xa11, 0, NULL, NULL, NULL, NULL)
    ){
        this->unk138_23 = TRUE;
    }
}

void TTC_Nipper_updateFunc(Actor *this){
    f32 particlePosition[3];
    s32 sp48;
    s32 xVelocity;
    f32 playerPosition[3];
    s32 temp_v0;

    player_getPosition(playerPosition);
    xVelocity = func_80309D58(playerPosition, 1);
    if(!this->volatile_initialized){
        this->volatile_initialized = TRUE;
        this->velocity_x = xVelocity;
        func_8032BC18(this);
    }

    if(this->state != TTC_NIPPER_STATE_7_UNKNOWN){
        if(0.0f == this->velocity_x && xVelocity){
            comusic_8025AB44(COMUSIC_12_TTC_NIPPER, -1, 5000);
            func_8032BB88(this, 0, 4000);
            func_8024BD08(0);
        }
        else if(!xVelocity && 0.0f != this->velocity_x){
            comusic_8025AB44(COMUSIC_12_TTC_NIPPER, 0, 300);
            func_8025AABC(COMUSIC_12_TTC_NIPPER);
            func_8032BB88(this, -1, 300);
            func_8024BD08(1);
        }
        this->velocity_x = xVelocity;
    }

    switch(this->state){
        case TTC_NIPPER_STATE_1_UNKNOWN:
            if(!this->initialized){
                animctrl_setTransitionDuration(this->animctrl, 0.35f);
                subaddie_set_state_with_direction(this, TTC_NIPPER_STATE_1_UNKNOWN, 0.01f, 1);
                this->lifetime_value = 120.0f;
                this->marker->propPtr->unk8_3 = TRUE;
                marker_setCollisionScripts(this->marker, TTC_Nipper_owFunc, TTC_Nipper_ow2Func, TTC_Nipper_dieFunc);
                func_803300C0(this->marker, TTC_Nipper_determineMarkeId);
                this->initialized = TRUE;
            }

            if(TTC_Nipper_shouldShowActor(this)){
                temp_v0 = func_8028ECAC();
                if( !this->has_met_before
                    && temp_v0 != 1
                    && temp_v0 != 10
                ){
                    subaddie_set_state_with_direction(this, TTC_NIPPER_STATE_SPAWNED, 0.01f, 1);
                    if(gcdialog_showText(ASSET_A0E_NIPPER_SPAWNED, 0xf, this->position, this->marker, TTC_Nipper_spawnedShowTextCallback, NULL)){
                        this->has_met_before = TRUE;
                    }
                    comusic_8025AB44(COMUSIC_12_TTC_NIPPER, 5000, 300);
                    ncStaticCamera_setToNode(11);
                }
                else{
                    TTC_Nipper_setAnimationDuration(this);
                }
                break;
            }
            
            if( actor_animationIsAt(this, 0.2f)
                || actor_animationIsAt(this, 0.2f)
                || actor_animationIsAt(this, 0.6f)
                || actor_animationIsAt(this, 0.7f)
            ){
                FUNC_8030E8B4(SFX_3D_TICKER_WALKING, 0.75f, 12000, this->position, 0x5dc, 0xbb8);
            }
            break;

        case TTC_NIPPER_STATE_2_UNKNOWN:
            if(!TTC_Nipper_shouldShowActor(this)){
                subaddie_set_state_looped(this, TTC_NIPPER_STATE_1_UNKNOWN);
                break;
            }

            if(this->lifetime_value <= this->unk38_31){
                TTC_Nipper_setAnimationDuration(this);
                break;
            }
            this->unk38_31++;
            break;

        case TTC_NIPPER_STATE_3_UNKNOWN:
            if(actor_animationIsAt(this, 0.5f) && this->marker->unk14_21){
                func_8034A174(this->marker->unk44, 6, particlePosition);
                TTC_Nipper_drawParticlesAtPosition(particlePosition, 2);
            }
            else if(actor_animationIsAt(this, 0.95f) && this->marker->unk14_21){//L80388800
                func_8034A174(this->marker->unk44, 5, particlePosition);
                TTC_Nipper_drawParticlesAtPosition(particlePosition, 2);
            }

            if(actor_animationIsAt(this, 0.99f)){
                this->unk1C[0] += 1.0f;
            }

            if(2.0f <= this->unk1C[0]){
                subaddie_set_state_looped(this, TTC_NIPPER_STATE_2_UNKNOWN);
                this->unk38_31 = 0;
                break;
            }

            if( actor_animationIsAt(this, 0.5f)
                || actor_animationIsAt(this, 0.95f) 
            ){
                for(sp48 = 0; sp48 < 3; sp48++){
                    FUNC_8030E8B4(SFX_3D_TICKER_WALKING, 0.75f, 12000, this->position, 1500, 3000);
                }
            }
            break;

        case TTC_NIPPER_STATE_DIEING:
            if(actor_animationIsAt(this, 0.99f)){
                TTC_Nipper_setAnimationDuration(this);
            }
            break;

        case TTC_NIPPER_STATE_SPAWNED:
            if( actor_animationIsAt(this, 0.2f)
                || actor_animationIsAt(this, 0.2f)
                || actor_animationIsAt(this, 0.6f)
                || actor_animationIsAt(this, 0.7f)
            ){
                FUNC_8030E8B4(SFX_3D_TICKER_WALKING, 0.75f, 12000, this->position, 0x5dc, 0xbb8);
            }
            break;

        case TTC_NIPPER_STATE_DEAD:
            this->marker->collidable = FALSE;
            if(actor_animationIsAt(this, 0.6f)){
                FUNC_8030E8B4(SFX_7C_CHEBOOF, 0.9f, 20000, this->position, 1500, 3000);
                break;
            }

            if(actor_animationIsAt(this, 0.99f)){
                subaddie_set_state_with_direction(this, TTC_NIPPER_STATE_7_UNKNOWN, 0.01f, 1);
            }
            break;

        case TTC_NIPPER_STATE_7_UNKNOWN:
            this->marker->collidable = FALSE;
            break;

    }//L80388A30
}

bool func_80388A44(s16 arg0[3]){
    f32 sp1C[3];
    Actor *nipper;

    sp1C[0] = (f32) arg0[0];
    sp1C[1] = (f32) arg0[1];
    sp1C[2] = (f32) arg0[2];

    nipper = actorArray_findClosestActorFromActorId(sp1C, ACTOR_117_NIPPER, -1, NULL);
    return nipper->state == TTC_NIPPER_STATE_7_UNKNOWN;
}
