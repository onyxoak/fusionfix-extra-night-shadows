#pragma once
#include <array>
#include <cstdint>
#include <cmath>

namespace fusionfix::shadows {
// Continuity participates in the native insertion sort, BEFORE static-cache
// scheduling. It never copies a cached map or manufactures an eligible light.
// Call Observe only for this frame's native-eligible inputs. Missing, invalid,
// out-of-range lights cannot be pinned by old identities. Relevance concerns
// the visible influence volume; an offscreen source can remain relevant.
class NativeShadowContinuity42 {
public:
    static constexpr unsigned Slots=7, Unclaimed=Slots;
    struct Identity {
        std::uint32_t key{};
        std::uint64_t generation{};
    };
    struct Candidate {
        std::uint32_t flags{};
        unsigned claim=Unclaimed;
        bool ownBeam=false;
        bool observed=false;
        std::uint32_t stableOrder=0; // Authenticated private lamp only; zero keeps native priority.
        float focusDistance=-1.0f; // Current player/camera score; negative means unavailable.
        unsigned focusTier=0;
        std::uint32_t identity=0; // Stable tie order, independent of producer traversal.
        float visualImportance=1.f; // Current authenticated receiver/photometric contribution.
    };
    void Begin(std::uintptr_t session,std::uint32_t frame,std::uint32_t now) noexcept {
        if(session!=session_ || frame<frame_ || now-time_>2000u) claims_={};
        session_=session;frame_=frame;time_=now;
        // A native update-divisor may skip passes. Keep the last successful set
        // through short gaps; abandon it after a pause/load, never by score.
        // Rank history must survive a slow frame/update-divisor gap. Observe
        // still requires a current native-eligible, relevant, matching identity;
        // this history can never restore a missing light or an old map.
        if(now-lastCommit_>2000u || frame-lastCommitFrame_>16u) claims_={};
    }
    Candidate Observe(Identity id,std::uint32_t flags,bool relevant,bool own) const noexcept {
        Candidate c{flags,Unclaimed,own,true};
        c.identity=id.key;
        if(id.key && relevant)
            for(unsigned i=0;i<Slots;++i)
                if(claims_[i].key==id.key && claims_[i].generation==id.generation) {c.claim=i;break;}
        return c;
    }
    bool Commit(const std::array<Identity,Slots>& chosen) noexcept {
        for(unsigned i=0;i<Slots;++i) if(chosen[i].key)
            for(unsigned j=0;j<i;++j) if(chosen[j].key==chosen[i].key) return false;
        claims_=chosen;lastCommit_=time_;lastCommitFrame_=frame_;return true;
    }
    // Native results: 0 inserts, 1 compares distance, 2 keeps the incumbent.
    // Preserve special native 0x400 priority. Own headlights stay usable on
    // entry/exit; other families share current relevance and bounded retention.
    static int Compare(int native,const Candidate& challenger,const Candidate& incumbent) noexcept {
        if(native<0 || native>2 || !challenger.observed || !incumbent.observed) return native;
        // Native 0x400 inserts every own beam ahead of its own prior beam.
        // Keep the two authenticated physical lamps in their drawn order;
        // still preserve special priority against every other light family.
        if(challenger.ownBeam && incumbent.ownBeam &&
           challenger.stableOrder && incumbent.stableOrder) {
            if(challenger.claim!=incumbent.claim) return challenger.claim<incumbent.claim?0:2;
            if(challenger.stableOrder!=incumbent.stableOrder) return challenger.stableOrder<incumbent.stableOrder?0:2;
        }
        if((challenger.flags|incumbent.flags)&0x400u) return native;
        // Only freshly authenticated physical forward lamps receive own-source
        // precedence. Native own vehicle keys can also represent tiny rear
        // emitters: those compete by receiver consequence like other lights.
        const bool challengerForward=challenger.ownBeam && challenger.stableOrder>0;
        const bool incumbentForward=incumbent.ownBeam && incumbent.stableOrder>0;
        if(challengerForward!=incumbentForward) return challengerForward?0:2;
        // A previously selected distant map must not pin out a new light
        // affecting the player or visible road. Retention is a bounded score
        // advantage, never permanent precedence across relevant sources.
        if(std::isfinite(challenger.focusDistance)&&challenger.focusDistance>=0&&
           std::isfinite(incumbent.focusDistance)&&incumbent.focusDistance>=0) {
            if(challenger.focusTier!=incumbent.focusTier)
                return challenger.focusTier>incumbent.focusTier?0:2;
            // Membership uses one score per candidate. Pairwise percentage
            // bands produced A beats B, B beats C, C beats A; shuffled native
            // traversal could then change the seven winners. Map ordinals are
            // stabilized separately at the authenticated pre-cache boundary.
            const float a=RetainedScore(challenger);
            const float b=RetainedScore(incumbent);
            if(a!=b)return a<b?0:2;
            if(challenger.claim!=incumbent.claim)return challenger.claim<incumbent.claim?0:2;
            if(challenger.identity && incumbent.identity && challenger.identity!=incumbent.identity)
                return challenger.identity<incumbent.identity?0:2;
        }
        if(challenger.claim!=incumbent.claim) return challenger.claim<incumbent.claim?0:2;
        return native;
    }
    static float RetainedScore(const Candidate& candidate) noexcept {
        if(candidate.claim>=Slots)return candidate.focusDistance;
        if(candidate.flags&0x100u)return candidate.focusDistance*.65f;
        const float impact=std::isfinite(candidate.visualImportance)?
            std::fmax(0.f,std::fmin(1.f,(candidate.visualImportance-1.f)*.75f)):0.f;
        // High-impact visible receiver shadows need a materially better
        // challenger before replacement. Minor effects keep build22's hold.
        // This is one scalar per candidate, preserving consistent ordering.
        return candidate.focusDistance*(.8f-.2f*impact);
    }
private:
    std::array<Identity,Slots> claims_{};
    std::uintptr_t session_{};
    std::uint32_t frame_{},time_{},lastCommit_{},lastCommitFrame_{};
};
}
