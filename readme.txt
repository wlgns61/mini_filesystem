노드구성: id, name, permission(기본값 변경불가), time, 링크 4가지(lsibling,rslbling,lchild,parent)

가장 기본으로 root->home->user 생성, 현재위치는 user

기능:
1. mkdir
 -> mkdir  경로 및 이름 : 디렉토리 생성
    mkdir -p 이름/이름/이름 : 현 위치부터 디렉토리를 연속해서 만듬

2. cd
 ->  cd .  : 현재위치그대로
     cd ..  :  상위 디렉토리로 이동
     cd ~ : user로 이동
     cd /  : root로 이동
     cd    : user로 이동
     cd    :경로 및 이름

3. pwd
->  pwd : root부터 자신의 경로를 보여줌

4. cat
->  cat > 경로 및 이름: 텍스트파일 생성 (:q 또는 :wq 입력시 종료)
    cat 경로 및 이름    : 택스트 파일 열기

5. rm
->  rm 경로 및 이름 : 파일 삭제
    rm -rf 이름        : 디렉토리 또는 파일의 하위 노드 전부 강제 삭제

6. ls
->  ls    : 현 디렉토리의 하위 목록들을 보여줌
    ls -l or -al: 좀 더 자세한 정보를 보여줌(권한, 노드 생성 시간, 사용자, 이름)

7.   rmdir 경로 및 파일: 디렉토리 삭제
     rmdir -p 경로 : 입력한 경로의 디렉토리를 삭제해 나감

8. mv a b 경로 : a라는 노드를 b로 이름을 바꿔 '경로'로 이동한다.

세마포 구현: multi processing시에 pid값을 출력

저장:
save함수를 통해 노드들을 preorder순으로 바이너리 파일로 저장(명령어 실행시 계속 저장)
load함수를 통해 바이너리 파일에 preorder순으로 저장된 노드들을 노드의 링크에 대한 정보를 이용해 다시 불러옴