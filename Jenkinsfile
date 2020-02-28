pipeline {
    agent {
        node {
            label 'master'
            customWorkspace '/home/havendev/src'
        }
    }

    stages {
        stage('Build') {
            steps {
                echo 'Doing a Clean Build...'
                sh '''
                make clean
                make
                '''
            }
        }
        stage('Test') {
            steps {
                echo 'Executing a Test Run...'
                dir('/home/havendev/area') {
                    sh '../src/rom 2002 testrun'
                }
            }
        }
    }
}
