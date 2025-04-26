from locust import HttpUser, TaskSet, task, between

class UserBehavior(TaskSet):
    @task(3)
    def load_main_page(self):
        self.client.get("/")

    @task(2)
    def load_second_page(self):
        self.client.get("/second_page.html")

class WebsiteUser(HttpUser):
    tasks = [UserBehavior]
    wait_time = between(1, 2)

