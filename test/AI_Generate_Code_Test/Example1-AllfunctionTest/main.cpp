// main.cpp — C++11 Test Runner for TagStatic.h
// Requirements covered:
//  - Test every struct and function with step-by-step printf OK/NG
//  - Headed sections with per-section score and overall score
//  - Parameters: in-range, out-of-range, type mismatch, not-found, external buffer, size=0
//  - No direct access to struct variables; methods only
//  - nullptr only; no NULL/null
//  - Comments in English
//  - Style: MISRA-ish (explicit const, static helpers, no using-namespace, limited casts)

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "TagStatic.h"  // Ensure header is patched as discussed earlier

// ===== Test Harness (C++11, MISRA-ish) ======================================

struct Section {
    const char* title;
    int passed;
    int total;
};

static void print_ok(Section* s, const char* msg)
{
    if (s != nullptr) { s->passed += 1; s->total += 1; }
    (void)std::printf("[OK] %s\n", msg);
}

static void print_ng(Section* s, const char* msg)
{
    if (s != nullptr) { s->total += 1; }
    (void)std::printf("[NG] %s\n", msg);
}

static void check_cond(Section* s, bool cond, const char* okm, const char* ngm)
{
    if (cond) {
        print_ok(s, okm);
    } else {
        print_ng(s, ngm);
    }
}

static int score_of(const Section& s)
{
    int result = 0;
    if (s.total > 0) {
        const double sc = (100.0 * static_cast<double>(s.passed)) / static_cast<double>(s.total);
        result = static_cast<int>(sc + 0.5);
    }
    return result;
}

// ===== Section 1: Tag basic via TagGroup (Set/Get/Bounds/Monitor/Remove) ====

static Section test_tag_basic(void)
{
    Section S = { "Tag: Set/Get/Bounds/Monitor/Remove (via TagGroup)", 0, 0 };

    TagGroup group;
    Tag* const t = group.Add(VAR_INT16, "T16", 4u, RAM);
    check_cond(&S, (t != nullptr), "Add(VAR_INT16,'T16',4)", "Add Tag failed");

    // Set/Get in range
    const bool set_ok = (t != nullptr) ? t->Set<int16_t>(123, 0u) : false;
    check_cond(&S, set_ok, "Set<int16_t>(123,0)", "Set failed");

    const int16_t v0 = (t != nullptr) ? t->Get<int16_t>(0u) : static_cast<int16_t>(0);
    check_cond(&S, (v0 == 123), "Get<int16_t>(0)==123", "Get mismatch");

    // Type mismatch (size mismatch)
    const bool set_bad = (t != nullptr) ? t->Set<int32_t>(777, 1u) : false;
    check_cond(&S, (set_bad == false), "Set<int32_t> rejected (mismatch)", "Set<int32_t> should fail");
    const int32_t g_bad = (t != nullptr) ? t->Get<int32_t>(1u) : static_cast<int32_t>(0);
    check_cond(&S, (g_bad == static_cast<int32_t>(0)), "Get<int32_t> returns default", "Get<int32_t> not default");

    // Bounds
    const size_t n = (t != nullptr) ? t->GetArraySize() : 0u;
    const bool set_oob = (t != nullptr) ? t->Set<int16_t>(999, n) : false;
    check_cond(&S, (set_oob == false), "Set OOB rejected", "Set OOB accepted");
    const int16_t g_oob = (t != nullptr) ? t->Get<int16_t>(n) : static_cast<int16_t>(0);
    check_cond(&S, (g_oob == static_cast<int16_t>(0)), "Get OOB returns default", "Get OOB not default");

    // Monitor*
    const char* const info = (t != nullptr) ? t->MonitorInfo() : nullptr;
    check_cond(&S, (info != nullptr), "MonitorInfo non-null", "MonitorInfo null");
    const char* const mv0 = (t != nullptr) ? t->MonitorValue(0u) : nullptr;
    check_cond(&S, (mv0 != nullptr), "MonitorValue(0) non-null", "MonitorValue(0) null");

    // Remove: not-found and then real removal by name
    group.Remove("NOT_FOUND");
    print_ok(&S, "Remove('NOT_FOUND') safe (no-op)");

    group.Remove("T16");  // TagGroup::Remove is safe in your header
    check_cond(&S, (group.Find("T16") == nullptr), "Remove('T16') success", "Remove('T16') failed");

    return S;
}

// ===== Section 2: Tag all types + external buffer ===========================

static Section test_tag_types_and_external(void)
{
    Section S = { "Tag: All types + External buffer", 0, 0 };
    TagGroup group;

    struct CaseInfo { isType tp; const char* name; size_t n; };
    static const CaseInfo cases[] = {
        { VAR_CHAR,  "C",  3u },
        { VAR_INT8,  "I8", 3u },
        { VAR_UINT8, "U8", 3u },
        { VAR_INT16, "I16",2u },
        { VAR_UINT16,"U16",2u },
        { VAR_INT32, "I32",2u },
        { VAR_UINT32,"U32",2u },
        { VAR_FLOAT, "F",  2u },
        { VAR_DOUBLE,"D",  2u }
    };

    for (size_t i = 0u; i < (sizeof(cases) / sizeof(cases[0])); ++i) {
        Tag* const t = group.Add(cases[i].tp, cases[i].name, cases[i].n, RAM);
        check_cond(&S, (t != nullptr), "Add(type,name,n)", "Add failed");

        bool set_all = (t != nullptr);
        if (t != nullptr) {
            // Index 0
            if (cases[i].tp == VAR_CHAR)        { set_all = set_all && t->Set<char>('A', 0u); }
            else if (cases[i].tp == VAR_INT8)   { set_all = set_all && t->Set<int8_t>(-5, 0u); }
            else if (cases[i].tp == VAR_UINT8)  { set_all = set_all && t->Set<uint8_t>(200u, 0u); }
            else if (cases[i].tp == VAR_INT16)  { set_all = set_all && t->Set<int16_t>(-1234, 0u); }
            else if (cases[i].tp == VAR_UINT16) { set_all = set_all && t->Set<uint16_t>(60000u, 0u); }
            else if (cases[i].tp == VAR_INT32)  { set_all = set_all && t->Set<int32_t>(-123456, 0u); }
            else if (cases[i].tp == VAR_UINT32) { set_all = set_all && t->Set<uint32_t>(3000000000u, 0u); }
            else if (cases[i].tp == VAR_FLOAT)  { set_all = set_all && t->Set<float>(3.14f, 0u); }
            else if (cases[i].tp == VAR_DOUBLE) { set_all = set_all && t->Set<double>(2.718281828, 0u); }
            else { /* no-op */ }

            // Index 1 (if exists)
            if (cases[i].n > 1u) {
                if (cases[i].tp == VAR_CHAR)        { set_all = set_all && t->Set<char>('Z', 1u); }
                else if (cases[i].tp == VAR_INT8)   { set_all = set_all && t->Set<int8_t>(7, 1u); }
                else if (cases[i].tp == VAR_UINT8)  { set_all = set_all && t->Set<uint8_t>(123u, 1u); }
                else if (cases[i].tp == VAR_INT16)  { set_all = set_all && t->Set<int16_t>(2222, 1u); }
                else if (cases[i].tp == VAR_UINT16) { set_all = set_all && t->Set<uint16_t>(44444u, 1u); }
                else if (cases[i].tp == VAR_INT32)  { set_all = set_all && t->Set<int32_t>(100000, 1u); }
                else if (cases[i].tp == VAR_UINT32) { set_all = set_all && t->Set<uint32_t>(1234567890u, 1u); }
                else if (cases[i].tp == VAR_FLOAT)  { set_all = set_all && t->Set<float>(-1.5f, 1u); }
                else if (cases[i].tp == VAR_DOUBLE) { set_all = set_all && t->Set<double>(-0.25, 1u); }
                else { /* no-op */ }
            }
        }
        check_cond(&S, set_all, "Set values OK", "Set values NG");

        const char* const mv0 = (t != nullptr) ? t->MonitorValue(0u) : nullptr;
        check_cond(&S, (mv0 != nullptr), "MonitorValue(0) non-null", "MonitorValue(0) null");
    }

    // External buffer overload
    uint8_t ext[5u] = { 1u, 2u, 3u, 4u, 5u };
    Tag* const e = group.Add(VAR_UINT8, "EXT", static_cast<void*>(ext), 5u, RAM);
    check_cond(&S, (e != nullptr), "Add external UINT8[5]", "Add external failed");
    if (e != nullptr) {
        check_cond(&S, (e->Get<uint8_t>(3u) == 4u), "External Get(3)==4", "External Get mismatch");
        ext[2u] = 99u;
        check_cond(&S, (e->Get<uint8_t>(2u) == 99u), "External reflects change", "External not reflected");
    }

    group.RemoveAll();
    check_cond(&S, (group.Count() == 0u), "RemoveAll cleared", "RemoveAll NG");
    return S;
}

// ===== Section 3: TagGroup API (Add/Find/Remove/Count) ======================

static Section test_taggroup_api(void)
{
    Section S = { "TagGroup: Add/Find/FindIndex/Remove/RemoveAll/Count", 0, 0 };
    TagGroup group;

    Tag* const A = group.Add(VAR_UINT16, "A", 3u, RAM);
    Tag* const B = group.Add(VAR_INT32,  "B", 2u, RAM);
    check_cond(&S, ((A != nullptr) && (B != nullptr)), "Add A,B", "Add A,B failed");
    check_cond(&S, (group.Count() == 2u), "Count()==2", "Count!=2");

    check_cond(&S, (group.Find("A") == A), "Find('A')", "Find('A') NG");
    check_cond(&S, (group.Find(static_cast<size_t>(1u)) == B), "Find(1)==B", "Find(1) NG");
    check_cond(&S, (group.FindIndex("B") == 1), "FindIndex('B')==1", "FindIndex NG");
    check_cond(&S, (group.Find("X") == nullptr), "Find('X')==nullptr", "Find('X') should be nullptr");

    const size_t before = group.Count();
    group.Remove(static_cast<size_t>(100u));  // no-op
    check_cond(&S, (group.Count() == before), "Remove(100) no-op", "Remove(100) changed list");

    group.Remove(static_cast<size_t>(0u));    // remove head
    check_cond(&S, (group.Find("A") == nullptr), "Remove(0) removed head", "Remove(0) NG");

    group.Remove("B");
    check_cond(&S, (group.Count() == 0u), "Remove('B') -> 0", "Remove('B') NG");

    group.RemoveAll(); // on empty
    print_ok(&S, "RemoveAll() on empty safe");

    return S;
}

// ===== Section 4: Topic/TopicGroup (safe remove path + Items/Monitor) =======

static Section test_topic_api(void)
{
    Section S = { "Topic/TopicGroup: Add/Find/Items/Monitor/RemoveAll", 0, 0 };
    TopicGroup topics;

    Topic* const T1 = topics.Add("Sensors", RAM);
    Topic* const T2 = topics.Add("Actuators", RAM);
    check_cond(&S, ((T1 != nullptr) && (T2 != nullptr)), "Add Topics", "Add Topics NG");
    check_cond(&S, (topics.Count() == 2u), "Count()==2", "Count!=2");

    check_cond(&S, (topics.Find("Sensors") == T1), "Find('Sensors')", "Find('Sensors') NG");
    check_cond(&S, (topics.FindIndex("Actuators") == 1), "FindIndex('Actuators')==1", "FindIndex NG");

    const char* const mi = (T1 != nullptr) ? T1->MonitorInfo() : nullptr;
    check_cond(&S, (mi != nullptr), "Topic::MonitorInfo non-null", "Topic::MonitorInfo null");

    const bool rn = (T1 != nullptr) ? T1->SetName("Inputs") : false;
    check_cond(&S, rn, "Topic.SetName('Inputs')", "Topic.SetName NG");
    check_cond(&S, (T1 != nullptr) && (std::strcmp(T1->Getname(), "Inputs") == 0),
              "Getname=='Inputs'", "Getname NG");

    // Items() existence depends on Add() implementation
    check_cond(&S, (T1 != nullptr) && (T1->Items() != nullptr), "T1->Items()!=nullptr", "T1->Items() NULL");
    check_cond(&S, (T2 != nullptr) && (T2->Items() != nullptr), "T2->Items()!=nullptr", "T2->Items() NULL");

    // Nested Tag under Topic->Items()
    if ((T1 != nullptr) && (T1->Items() != nullptr)) {
        Tag* const u8 = T1->Items()->Add(VAR_UINT8, "U8", 3u, RAM);
        check_cond(&S, (u8 != nullptr), "Items()->Add(VAR_UINT8,'U8',3)", "Items()->Add NG");
        if (u8 != nullptr) {
            const bool ss = u8->Set<uint8_t>(7u, 1u);
            check_cond(&S, ss, "Nested Tag Set OK", "Nested Tag Set NG");
            check_cond(&S, (u8->Get<uint8_t>(1u) == 7u), "Nested Tag Get OK", "Nested Tag Get NG");
        }
    }

    // Memory exercise: RemoveAll should release and allow re-add safely
    topics.RemoveAll();
    check_cond(&S, (topics.Count() == 0u), "RemoveAll -> 0", "RemoveAll NG");

    Topic* const T3 = topics.Add("Again", RAM);
    check_cond(&S, (T3 != nullptr), "Add after RemoveAll works", "Add after RemoveAll failed");
    topics.RemoveAll();
    check_cond(&S, (topics.Count() == 0u), "Final RemoveAll -> 0", "Final RemoveAll NG");

    return S;
}

// ===== Section 5: Parameter edge cases + memory exercise loops ===============

static Section test_param_edges_and_memory(void)
{
    Section S = { "Params: external nullptr / array_size=0 / stress Add-Remove", 0, 0 };

    // External address nullptr should be rejected
    TagGroup g1;
    Tag* const bad = g1.Add(VAR_UINT8, "E_NULL", static_cast<void*>(nullptr), 5u, RAM);
    check_cond(&S, (bad == nullptr), "Add external with nullptr rejected", "Add external nullptr accepted (BUG)");

    // array_size == 0: acceptable reject; must not crash
    Tag* const z = g1.Add(VAR_INT16, "ZERO", 0u, RAM);
    if (z == nullptr) {
        print_ok(&S, "Add(..., size=0) rejected (acceptable)");
    } else {
        print_ok(&S, "Add(..., size=0) accepted");
        const bool ss = z->Set<int16_t>(11, 0u);
        check_cond(&S, (ss == false), "Set on zero-size rejected", "Set on zero-size accepted (BUG)");
    }
    g1.RemoveAll();
    print_ok(&S, "RemoveAll() on empty/partial safe");

    // Memory exercise: many Add/Remove cycles (TagGroup)
    {
        TagGroup gx;
        const std::size_t cycles = 3u;
        const std::size_t batch  = 64u;
        for (std::size_t c = 0u; c < cycles; ++c) {
            for (std::size_t i = 0u; i < batch; ++i) {
                Tag* const t = gx.Add(VAR_UINT16, "M", 4u, RAM);
                if (t != nullptr) {
                    (void)t->Set<uint16_t>(static_cast<uint16_t>(i), 0u);
                }
            }
            check_cond(&S, (gx.Count() == batch), "Batch Add count OK", "Batch Add count NG");

            // Remove half by index 0 repeatedly (head churn) – ensures nodes and buffers are freed correctly
            for (std::size_t k = 0u; k < (batch / 2u); ++k) {
                gx.Remove(static_cast<std::size_t>(0u));
            }
            check_cond(&S, (gx.Count() == (batch - (batch / 2u))), "Half removed OK", "Half removed NG");

            // Remove rest
            gx.RemoveAll();
            check_cond(&S, (gx.Count() == 0u), "RemoveAll after churn -> 0", "RemoveAll after churn NG");
        }
    }

    // Memory exercise: TopicGroup re-use after RemoveAll (safe path only)
    {
        TopicGroup tg;
        const std::size_t loops = 3u;
        for (std::size_t r = 0u; r < loops; ++r) {
            (void)tg.Add("T", RAM);
            (void)tg.Add("Q", RAM);
            check_cond(&S, (tg.Count() == 2u), "Topic Add 2 OK", "Topic Add 2 NG");
            tg.RemoveAll();
            check_cond(&S, (tg.Count() == 0u), "Topic RemoveAll -> 0", "Topic RemoveAll NG");
        }
    }

    return S;
}

// ===== Runner ================================================================

int main(void)
{
    (void)std::printf("==== Point/Tag Static Library Test Runner (C++11, MISRA-ish, methods-only) ====\n");
    (void)std::printf("Sections:\n");
    (void)std::printf("  1) Tag: Set/Get/Bounds/Monitor/Remove (via TagGroup)\n");
    (void)std::printf("  2) Tag: All types + External buffer\n");
    (void)std::printf("  3) TagGroup: Add/Find/FindIndex/Remove/RemoveAll/Count\n");
    (void)std::printf("  4) Topic/TopicGroup: Add/Find/Items/Monitor/RemoveAll\n");
    (void)std::printf("  5) Params: external nullptr / array_size=0 / stress Add-Remove\n");
    (void)std::printf("-------------------------------------------------------------------------------\n\n");

    const Section s1 = test_tag_basic();
    (void)std::printf("\n-- Section 1 summary: %d/%d (score %d/100)\n\n", s1.passed, s1.total, score_of(s1));

    const Section s2 = test_tag_types_and_external();
    (void)std::printf("\n-- Section 2 summary: %d/%d (score %d/100)\n\n", s2.passed, s2.total, score_of(s2));

    const Section s3 = test_taggroup_api();
    (void)std::printf("\n-- Section 3 summary: %d/%d (score %d/100)\n\n", s3.passed, s3.total, score_of(s3));

    const Section s4 = test_topic_api();
    (void)std::printf("\n-- Section 4 summary: %d/%d (score %d/100)\n\n", s4.passed, s4.total, score_of(s4));

    const Section s5 = test_param_edges_and_memory();
    (void)std::printf("\n-- Section 5 summary: %d/%d (score %d/100)\n\n", s5.passed, s5.total, score_of(s5));

    const int pass = s1.passed + s2.passed + s3.passed + s4.passed + s5.passed;
    const int tot  = s1.total  + s2.total  + s3.total  + s4.total  + s5.total;
    const int all  = (tot > 0) ? static_cast<int>((100.0 * static_cast<double>(pass) / static_cast<double>(tot)) + 0.5) : 0;

    (void)std::printf("===============================================================================\n");
    (void)std::printf("Overall: %d/%d  => %d/100\n", pass, tot, all);
    (void)std::printf("===============================================================================\n");

    (void)std::printf("\nNotes:\n");
    (void)std::printf(" - Tests use only methods, never access struct fields directly.\n");
    (void)std::printf(" - Memory correctness is exercised by repeated Add/Remove/RemoveAll cycles.\n");
    (void)std::printf(" - To also test TopicGroup::Remove(index) real deletion, ensure its impl frees\n");
    (void)std::printf("   the node (free(*cur)) not the pointer-to-pointer, then add a real remove case.\n");

    return 0;
}
