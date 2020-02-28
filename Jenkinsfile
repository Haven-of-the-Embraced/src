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
                echo 'Building...'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing...'
            }
        }
    }
}
