# delete num=1
```
cypher: Maintenance match (m:Place)<-[*0]-(VD)<-[VR]-(VS)<-[*1..1]-(n:Comment) where euid(VR) in ['2962874_1873113_3_0_1'] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
Union 
match (m:Place)<-[*1]-(VD)<-[VR]-(VS)<-[*0..0]-(n:Comment) where euid(VR) in ['2962874_1873113_3_0_1'] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
total db hit:37
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Variable Length Expand(All) [VS <--*1..1 n]{
                    node cnt:
                    layer:0,count:0
                    layer:1,count:0
                    expand cnt:
                    layer:0,count:0
                    } (0 rows, 0 hits)
                        Variable Length Expand(All) [VD -->*0..0 m]{
                        node cnt:
                        layer:0,count:0
                        expand cnt:
                        } (0 rows, 1 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [2962874_1873113_3_0_1]}),0.000000,0.000010] (1 rows, 6 hits)
                                Node By Id Seek[2962874,] (1 rows)
        Delete (1 rows)
            Apply (1 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (1 rows, 6 hits)
                    Argument [n,m,num] (1 rows)
                Aggregate [n,m,num] (1 rows)
                    Variable Length Expand(All) [VS <--*0..0 n]{
                    node cnt:
                    layer:0,count:0
                    expand cnt:
                    } (1 rows, 1 hits)
                        Variable Length Expand(All) [VD -->*1..1 m]{
                        node cnt:
                        layer:0,count:0
                        layer:1,count:5
                        expand cnt:
                        layer:0,count:6
                        } (1 rows, 13 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [2962874_1873113_3_0_1]}),0.000000,0.000006] (1 rows, 6 hits)
                                Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Comment)-[:replyOf*0..]->(VS)-[VR:replyOf]->(VD)-[:replyOf*0..]->(m:Post) where euid(VR) in ['2962874_1873113_3_0_1'] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:ROOT_POST NoDupEdge ]->(m) delete r
total db hit:14
Profile statistics:
Produce Results (1 rows)
    Delete (1 rows)
        Apply (1 rows)
            Expand(Into) [n --> m ,0.000000,0.000000] (1 rows, 4 hits)
                Argument [n,m,num] (1 rows)
            Aggregate [n,m,num] (1 rows)
                Variable Length Expand(All) [VD -->*0..128 m]{
                } (1 rows, 2 hits)
                    Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [2962874_1873113_3_0_1]}),0.000000,0.000007] (1 rows, 4 hits)
                        Variable Length Expand(All) [VS <--*0..128 n]{
                        } (1 rows, 2 hits)
                            Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Person)-[VR]->(ANON_N0)-[r2]->(m:Place) where euid(VR) in ['2962874_1873113_3_0_1'] and id(n) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
Union 
match (n:Person)-[r1]->(ANON_N1)-[VR]->(m:Place) where euid(VR) in ['2962874_1873113_3_0_1'] and id(ANON_N1) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
total db hit:5
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N0 --> m ,0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [n --> ANON_N0 EdgeFilter ({euid(false,VR) IN [2962874_1873113_3_0_1]}),0.000000,0.000000] (0 rows, 0 hits)
                            Node By Id Seek[2962874,] (0 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N1 --> m EdgeFilter ({euid(false,VR) IN [2962874_1873113_3_0_1]}),0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [ANON_N1 <-- n ,0.000000,0.000000] (0 rows, 1 hits)
                            Node By Id Seek[2962874,] (1 rows)



```
# delete num=10
```
cypher: Maintenance match (m:Place)<-[*0]-(VD)<-[VR]-(VS)<-[*1..1]-(n:Comment) where euid(VR) in [10 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
Union 
match (m:Place)<-[*1]-(VD)<-[VR]-(VS)<-[*0..0]-(n:Comment) where euid(VR) in [10 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
total db hit:226
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Variable Length Expand(All) [VS <--*1..1 n]{
                    node cnt:
                    layer:0,count:0
                    layer:1,count:0
                    expand cnt:
                    layer:0,count:0
                    } (0 rows, 0 hits)
                        Variable Length Expand(All) [VD -->*0..0 m]{
                        node cnt:
                        layer:0,count:0
                        expand cnt:
                        } (0 rows, 10 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10 euids]}),0.000000,0.000014] (10 rows, 24 hits)
                                Node By Id Seek[2962874,] (1 rows)
        Delete (1 rows)
            Apply (10 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (10 rows, 24 hits)
                    Argument [n,m,num] (1 rows)
                Aggregate [n,m,num] (1 rows)
                    Variable Length Expand(All) [VS <--*0..0 n]{
                    node cnt:
                    layer:0,count:0
                    expand cnt:
                    } (10 rows, 10 hits)
                        Variable Length Expand(All) [VD -->*1..1 m]{
                        node cnt:
                        layer:0,count:0
                        layer:1,count:50
                        expand cnt:
                        layer:0,count:60
                        } (10 rows, 130 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10 euids]}),0.000000,0.000012] (10 rows, 24 hits)
                                Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Comment)-[:replyOf*0..]->(VS)-[VR:replyOf]->(VD)-[:replyOf*0..]->(m:Post) where euid(VR) in [10 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:ROOT_POST NoDupEdge ]->(m) delete r
total db hit:68
Profile statistics:
Produce Results (1 rows)
    Delete (1 rows)
        Apply (10 rows)
            Expand(Into) [n --> m ,0.000000,0.000000] (10 rows, 22 hits)
                Argument [n,m,num] (1 rows)
            Aggregate [n,m,num] (1 rows)
                Variable Length Expand(All) [VD -->*0..128 m]{
} (10 rows, 20 hits)
                    Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10 euids]}),0.000000,0.000012] (10 rows, 22 hits)
                        Variable Length Expand(All) [VS <--*0..128 n]{
} (1 rows, 2 hits)
                            Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Person)-[VR]->(ANON_N0)-[r2]->(m:Place) where euid(VR) in [10 euids] and id(n) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
Union 
match (n:Person)-[r1]->(ANON_N1)-[VR]->(m:Place) where euid(VR) in [10 euids] and id(ANON_N1) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
total db hit:5
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N0 --> m ,0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [n --> ANON_N0 EdgeFilter ({euid(false,VR) IN [10 euids]}),0.000000,0.000000] (0 rows, 0 hits)
                            Node By Id Seek[2962874,] (0 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N1 --> m EdgeFilter ({euid(false,VR) IN [10 euids]}),0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [ANON_N1 <-- n ,0.000000,0.000000] (0 rows, 1 hits)
                            Node By Id Seek[2962874,] (1 rows)



```
# delete num=100
```
cypher: Maintenance match (m:Place)<-[*0]-(VD)<-[VR]-(VS)<-[*1..1]-(n:Comment) where euid(VR) in [100 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
Union 
match (m:Place)<-[*1]-(VD)<-[VR]-(VS)<-[*0..0]-(n:Comment) where euid(VR) in [100 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
total db hit:2116
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Variable Length Expand(All) [VS <--*1..1 n]{
                    node cnt:
                    layer:0,count:0
                    layer:1,count:0
                    expand cnt:
                    layer:0,count:0
                    } (0 rows, 0 hits)
                        Variable Length Expand(All) [VD -->*0..0 m]{
                        node cnt:
                        layer:0,count:0
                        expand cnt:
                        } (0 rows, 100 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [100 euids]}),0.000000,0.000485] (100 rows, 204 hits)
                                Node By Id Seek[2962874,] (1 rows)
        Delete (1 rows)
            Apply (100 rows)
                Expand(Into) [n --> m ,0.000000,0.000002] (100 rows, 204 hits)
                    Argument [n,m,num] (1 rows)
                Aggregate [n,m,num] (1 rows)
                    Variable Length Expand(All) [VS <--*0..0 n]{
                    node cnt:
                    layer:0,count:0
                    expand cnt:
                    } (100 rows, 100 hits)
                        Variable Length Expand(All) [VD -->*1..1 m]{
                        node cnt:
                        layer:0,count:0
                        layer:1,count:500
                        expand cnt:
                        layer:0,count:600
                        } (100 rows, 1300 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [100 euids]}),0.000000,0.000547] (100 rows, 204 hits)
                                Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Comment)-[:replyOf*0..]->(VS)-[VR:replyOf]->(VD)-[:replyOf*0..]->(m:Post) where euid(VR) in [100 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:ROOT_POST NoDupEdge ]->(m) delete r
total db hit:608
Profile statistics:
Produce Results (1 rows)
    Delete (1 rows)
        Apply (100 rows)
            Expand(Into) [n --> m ,0.000000,0.000002] (100 rows, 202 hits)
                Argument [n,m,num] (1 rows)
            Aggregate [n,m,num] (1 rows)
                Variable Length Expand(All) [VD -->*0..128 m]{
                } (100 rows, 200 hits)
                    Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN []}),0.000000,0.000475] (100 rows, 202 hits)
                        Variable Length Expand(All) [VS <--*0..128 n]{
                        } (1 rows, 2 hits)
                            Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Person)-[VR]->(ANON_N0)-[r2]->(m:Place) where euid(VR) in [] and id(n) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
Union 
match (n:Person)-[r1]->(ANON_N1)-[VR]->(m:Place) where euid(VR) in [] and id(ANON_N1) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
total db hit:5
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N0 --> m ,0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [n --> ANON_N0 EdgeFilter ({euid(false,VR) IN []}),0.000000,0.000000] (0 rows, 0 hits)
                            Node By Id Seek[2962874,] (0 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N1 --> m EdgeFilter ({euid(false,VR) IN []}),0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [ANON_N1 <-- n ,0.000000,0.000000] (0 rows, 1 hits)
                            Node By Id Seek[2962874,] (1 rows)



```
# delete num=1000
```
cypher: Maintenance match (m:Place)<-[*0]-(VD)<-[VR]-(VS)<-[*1..1]-(n:Comment) where euid(VR) in [1000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
Union 
match (m:Place)<-[*1]-(VD)<-[VR]-(VS)<-[*0..0]-(n:Comment) where euid(VR) in [1000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
total db hit:21016
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Variable Length Expand(All) [VS <--*1..1 n]{
                    node cnt:
                    layer:0,count:0
                    layer:1,count:0
                    expand cnt:
                    layer:0,count:0
                    } (0 rows, 0 hits)
                        Variable Length Expand(All) [VD -->*0..0 m]{
                        node cnt:
                        layer:0,count:0
                        expand cnt:
                        } (0 rows, 1000 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [1000 euids]}),0.000000,0.052806] (1000 rows, 2004 hits)
                                Node By Id Seek[2962874,] (1 rows)
        Delete (1 rows)
            Apply (1000 rows)
                Expand(Into) [n --> m ,0.000000,0.000022] (1000 rows, 2004 hits)
                    Argument [n,m,num] (1 rows)
                Aggregate [n,m,num] (1 rows)
                    Variable Length Expand(All) [VS <--*0..0 n]{
                    node cnt:
                    layer:0,count:0
                    expand cnt:
                    } (1000 rows, 1000 hits)
                        Variable Length Expand(All) [VD -->*1..1 m]{
                        node cnt:
                        layer:0,count:0
                        layer:1,count:5000
                        expand cnt:
                        layer:0,count:6000
                        } (1000 rows, 13000 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [1000 euids]}),0.000000,0.052425] (1000 rows, 2004 hits)
                                Node By Id Seek[2962874,] (1 rows)

cypher: Maintenance match (n:Comment)-[:replyOf*0..]->(VS)-[VR:replyOf]->(VD)-[:replyOf*0..]->(m:Post) where euid(VR) in [1000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:ROOT_POST NoDupEdge ]->(m) delete r
total db hit:6008
Profile statistics:
Produce Results (1 rows)
    Delete (1 rows)
        Apply (1000 rows)
            Expand(Into) [n --> m ,0.000000,0.000022] (1000 rows, 2002 hits)
                Argument [n,m,num] (1 rows)
            Aggregate [n,m,num] (1 rows)
                Variable Length Expand(All) [VD -->*0..128 m]{
                } (1000 rows, 2000 hits)
                    Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [1000 euids]}),0.000000,0.054129] (1000 rows, 2002 hits)
                        Variable Length Expand(All) [VS <--*0..128 n]{
                        } (1 rows, 2 hits)
                            Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Person)-[VR]->(ANON_N0)-[r2]->(m:Place) where euid(VR) in [1000 euids] and id(n) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
Union 
match (n:Person)-[r1]->(ANON_N1)-[VR]->(m:Place) where euid(VR) in [1000 euids] and id(ANON_N1) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
total db hit:5
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N0 --> m ,0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [n --> ANON_N0 EdgeFilter ({euid(false,VR) IN [1000 euids]}),0.000000,0.000000] (0 rows, 0 hits)
                            Node By Id Seek[2962874,] (0 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N1 --> m EdgeFilter ({euid(false,VR) IN [1000 euids]}),0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [ANON_N1 <-- n ,0.000000,0.000000] (0 rows, 1 hits)
                            Node By Id Seek[2962874,] (1 rows)
```
# delete num=10000
```
cypher: Maintenance match (m:Place)<-[*0]-(VD)<-[VR]-(VS)<-[*1..1]-(n:Comment) where euid(VR) in [10000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
Union 
match (m:Place)<-[*1]-(VD)<-[VR]-(VS)<-[*0..0]-(n:Comment) where euid(VR) in [10000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:COMMENT_PLACE NoDupEdge ]->(m) delete r
total db hit:210016
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Variable Length Expand(All) [VS <--*1..1 n]{
                    node cnt:
                    layer:0,count:0
                    layer:1,count:0
                    expand cnt:
                    layer:0,count:0
                    } (0 rows, 0 hits)
                        Variable Length Expand(All) [VD -->*0..0 m]{
                        node cnt:
                        layer:0,count:0
                        expand cnt:
                        } (0 rows, 10000 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10000 euids]}),0.000000,4.649635] (10000 rows, 20004 hits)
                                Node By Id Seek[2962874,] (1 rows)
        Delete (1 rows)
            Apply (10000 rows)
                Expand(Into) [n --> m ,0.000000,0.000170] (10000 rows, 20004 hits)
                    Argument [n,m,num] (1 rows)
                Aggregate [n,m,num] (1 rows)
                    Variable Length Expand(All) [VS <--*0..0 n]{
                    node cnt:
                    layer:0,count:0
                    expand cnt:
                    } (10000 rows, 10000 hits)
                        Variable Length Expand(All) [VD -->*1..1 m]{
                        node cnt:
                        layer:0,count:0
                        layer:1,count:50000
                        expand cnt:
                        layer:0,count:60000
                        } (10000 rows, 130000 hits)
                            Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10000 euids]}),0.000000,4.544529] (10000 rows, 20004 hits)
                                Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Comment)-[:replyOf*0..]->(VS)-[VR:replyOf]->(VD)-[:replyOf*0..]->(m:Post) where euid(VR) in [10000 euids] and id(VS) in [2962874]  WITH n,m,count(*) as num match (n)-[r:ROOT_POST NoDupEdge ]->(m) delete r
total db hit:60008
Profile statistics:
Produce Results (1 rows)
    Delete (1 rows)
        Apply (10000 rows)
            Expand(Into) [n --> m ,0.000000,0.000163] (10000 rows, 20002 hits)
                Argument [n,m,num] (1 rows)
            Aggregate [n,m,num] (1 rows)
                Variable Length Expand(All) [VD -->*0..128 m]{
                } (10000 rows, 20000 hits)
                    Expand(All) [VS --> VD EdgeFilter ({euid(false,VR) IN [10000 euids]}),0.000000,4.608654] (10000 rows, 20002 hits)
                        Variable Length Expand(All) [VS <--*0..128 n]{
                        } (1 rows, 2 hits)
                            Node By Id Seek[2962874,] (1 rows)


cypher: Maintenance match (n:Person)-[VR]->(ANON_N0)-[r2]->(m:Place) where euid(VR) in [10000 euids] and id(n) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
Union 
match (n:Person)-[r1]->(ANON_N1)-[VR]->(m:Place) where euid(VR) in [10000 euids] and id(ANON_N1) in [2962874]  WITH n,m,count(*) as num match (n)-[r:PERSON_PLACE NoDupEdge ]->(m) delete r
total db hit:5
Profile statistics:
Produce Results (2 rows)
    Union (2 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N0 --> m ,0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [n --> ANON_N0 EdgeFilter ({euid(false,VR) IN [10000 uids]}),0.000000,0.000000] (0 rows, 0 hits)
                            Node By Id Seek[2962874,] (0 rows)
        Delete (1 rows)
            Apply (0 rows)
                Expand(Into) [n --> m ,0.000000,0.000000] (0 rows, 0 hits)
                    Argument [n,m,num] (0 rows)
                Aggregate [n,m,num] (0 rows)
                    Expand(All) [ANON_N1 --> m EdgeFilter ({euid(false,VR) IN [10000 euids]}),0.000000,0.000000] (0 rows, 0 hits)
                        Expand(All) [ANON_N1 <-- n ,0.000000,0.000000] (0 rows, 1 hits)
                            Node By Id Seek[2962874,] (1 rows)
```

