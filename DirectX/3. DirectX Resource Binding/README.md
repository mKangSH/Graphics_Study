# 리소스 바인딩 (Resource Binding)
- 낮은 오버헤드, 자주 발생하는 API 호출에 최적화 됨
- 낮은 수준의 하드웨어에서 높은 수준의 하드웨어까지 확장 가능
- 셰이더 리소스는 셰이더 파이프라인에 설명자를 통해 참조
1. 설명자 (Descriptor)
    - 하나의 리소스(질감, 상수 테이블, 이미지, 버퍼 등)에 대한 정보를 포함하는 작은 개체 
2. 설명자 테이블 (Descriptor Table)
    - 그룹화된 설명자로 구성된 집합
    - 리소스 유형 중 한가지 범위에 대한 정보 저장
        - CBVs (Constant Buffer Views)
        - UAVs (Unordered Access Views)
        - SRVs (Shader Resource Views)
        - 샘플러 (Sampler) 
3. 설명자 힙 (Descriptor Heap)
    - 설명자 테이블로 구성된 집합
    - 렌더링될 하나 이상의 프레임에 대해 설명자 테이블에 모든 설명자를 포함
4. 루트 서명 (Root Signature)
    - 앱에서 정의된 바인딩 규칙
    - 셰이더가 엑세스 해야 하는 리소스를 찾는 데 사용
        - 그리기마다 변경되는 소량의 데이터에 적합한 성능 최적화 정보 직접 바인딩
        - 설명자 힙의 레이아웃이 미리 정의된 힙 내부 설명자 테이블에 대한 인덱스 
        - 사용자 정의 상수(루트 상수: Root constants)
        - 그리기마다 변경되는 CBV와 같은 매우 적은 수의 설명자